#include "ATACViewPlugin.h"

#include <event/Event.h>

#include <DatasetsMimeData.h>

#include <QDebug>
#include <QMimeData>

Q_PLUGIN_METADATA(IID "studio.manivault.ATACViewPlugin")

using namespace mv;

ATACViewPlugin::ATACViewPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _settingsAction(this, "Settings")
{
    connect(&mv::projects(), &AbstractProjectManager::projectOpened, this, [this]() {
        addProjectAveragesSettingsGroupActions();
        addOtherActions();
        });

    connect(&_settingsAction.getShowAdvancedSettingsAction(), &ToggleAction::toggled, this, [this](bool checked) {
        //qDebug() << "Show advanced settings toggled:" << checked;

        if (!_groupForAction || !_groupActionAdvanced)
        {
            //qDebug() << "Group for action or advanced group action is not initialized";
            return; // safety check
        }
            
        
        if (checked)
            _groupForAction->addGroupAction(_groupActionAdvanced);
        else
            _groupForAction->removeGroupAction(_groupActionAdvanced);

        //qDebug() << "Advanced settings group action" << (checked ? "added to" : "removed from") << "group for action";

        });
}

void ATACViewPlugin::init()
{
    // Create layout
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    _groupForAction = new GroupsAction(this, "Settings");

    layout->addWidget(_groupForAction->createWidget(&getWidget()));

    // Apply the layout
    getWidget().setLayout(layout);

    connect(&_pcaOutputDataset, &Dataset<Points>::dataChanged, this, [this]() {
        //qDebug() << "PCA output dataset data changed";
        
        // assume that PCA finished
        projectPC();
        });

    connect(&_projectionOutputDataset, &Dataset<Points>::dataChanged, this, [this]() {
        //qDebug() << "Projection output dataset data changed";
        
        // assume that projection finished
        plotPCProjection();
        });

    /*connect(&mv::plugins(), &AbstractPluginManager::pluginAdded, this, [this]() {
        _settingsAction.setupscatterplotForPCAction();
        });*/ // TODO: remove, not needed now

    if (!mv::projects().isOpeningProject())
    { 
        addProjectAveragesSettingsGroupActions();
        addOtherActions();
    }
}

void ATACViewPlugin::computePCA()
{
    mv::Dataset<Points> featureDataset;

    const QString featureMatrix = _settingsAction.getFeatureOptionAction().getCurrentText();
    if (featureMatrix == "ATAC")
        featureDataset = _settingsAction.getATACAveragesDatasetAction().getCurrentDataset<Points>();
    else if (featureMatrix == "RNA")
        featureDataset = _settingsAction.getRNAAveragesDatasetAction().getCurrentDataset<Points>();

    //auto featureDataset = _settingsAction.getFeatureDatasetAction().getCurrentDataset<Points>();
    
    if (!featureDataset.isValid())
        {
        qDebug() << "No valid feature dataset selected for PCA computation";
        QMessageBox::warning(nullptr, "Warning", "No valid feature dataset selected for PCA computation");
        return;
    }

    std::vector<bool> enabledDimensions = _settingsAction.getDimensionSelectionAction().getPickerAction().getEnabledDimensions();
    const auto numEnabledDimensions = std::count_if(enabledDimensions.begin(), enabledDimensions.end(), [](bool b) { return b; });
    qDebug() << "numEnabledDimensions = " << numEnabledDimensions << "length enabledDimensions = " <<enabledDimensions.size();
   
    // resize output data
    std::vector<float> data;
    std::vector<unsigned int> dimensionIndices;
    data.resize((featureDataset->isFull() ? featureDataset->getNumPoints() : featureDataset->indices.size()) * numEnabledDimensions);

    // populate dimensionIndices
    for (uint32_t i = 0; i < featureDataset->getNumDimensions(); i++)
        if (enabledDimensions[i])
            dimensionIndices.push_back(i);

    // populate data
    featureDataset->populateDataForDimensions<std::vector<float>, std::vector<unsigned int>>(data, dimensionIndices);

    if (!_pcaInputDataset.isValid()) {
        _pcaInputDataset = mv::data().createDataset("Points", "Dataset for PCA");
        //_pcaInputDataset->setGroupIndex(667);
        mv::events().notifyDatasetAdded(_pcaInputDataset);
    }
    _pcaInputDataset->setData(data.data(), featureDataset->getNumPoints(), dimensionIndices.size());

    if (!_pcaOutputDataset.isValid()) {
        _pcaOutputDataset = mv::data().createDerivedDataset("PCA 1", featureDataset);
        //_pcaOutputDataset->setGroupIndex(667);
        mv::events().notifyDatasetAdded(_pcaOutputDataset);
    }
    
    _computation.triggerPCA(_pcaInputDataset, _pcaOutputDataset);
}

void ATACViewPlugin::projectPC()
{
    std::vector<float> projectedData;
    _pcaOutputDataset->extractDataForDimension(projectedData, 0); // PC1

    auto spatialDataset = _settingsAction.getSpatialDatasetAction().getCurrentDataset<Points>();
    auto positionClusterDataset = _settingsAction.getSpatialClusterDatasetAction().getCurrentDataset<Clusters>();

    //auto averageClusterDataset = _settingsAction.getAveragesClusterDatasetAction().getCurrentDataset <Clusters>();
    mv::Dataset<Clusters> featureClusterDataset;
    const QString featureMatrix = _settingsAction.getFeatureOptionAction().getCurrentText();
    if (featureMatrix == "ATAC")
        featureClusterDataset = _settingsAction.getATACClusterDatasetAction().getCurrentDataset<Clusters>();
    else if (featureMatrix == "RNA")
        featureClusterDataset = _settingsAction.getRNAClusterDatasetAction().getCurrentDataset<Clusters>();

    if (!_projectionOutputDataset.isValid()) {
        _projectionOutputDataset = mv::data().createDerivedDataset("Mapped PC dataset", spatialDataset);
        //_pcaOutputDataset->setGroupIndex(667);
        mv::events().notifyDatasetAdded(_projectionOutputDataset);
    }

    if (!spatialDataset.isValid() || !positionClusterDataset.isValid() || !_pcaOutputDataset.isValid()
        || !featureClusterDataset.isValid() ) {
        qDebug() << "One or more required datasets for projecting PC are not valid";
        return;
    }

    _computation.triggerProjectAverages(spatialDataset, _projectionOutputDataset, _pcaOutputDataset, featureClusterDataset, positionClusterDataset);
    
}

void ATACViewPlugin::plotPCProjection()
{    
    // TODO: remove
    //QString scatterplotNameForPC = _settingsAction.getScatterplotForPCAction().getCurrentText();

    /*if (scatterplotNameForPC.isEmpty())
    {
        qWarning() << "No scatterplot option selected for plotting PC projection";
    }*/

    auto spatialDataset = _settingsAction.getSpatialDatasetAction().getCurrentDataset<Points>(); // coordinates
    const QString positionDatasetID = spatialDataset.getDatasetId();
    
    const QString colorDatasetID = _projectionOutputDataset.getDatasetId();

    auto celltypeClusterDataset = _settingsAction.getCellTypeDatasetAction().getCurrentDataset<Clusters>();
    const QString celltypeClusterDatasetID = celltypeClusterDataset.getDatasetId();

    // create opacity dataset
    if (!_opacityDataset.isValid())
    {
        _opacityDataset = mv::data().createDerivedDataset("Opacity scalars celltype", spatialDataset);

        //_opacityDataset->setGroupIndex(667);
        mv::events().notifyDatasetAdded(_opacityDataset);
    }

    // set opacity scalars to 0 for non-target cell types and 1 for target cell type
    QStringList selectedClusterNamesList = _settingsAction.getCellTypeSelectionAction().getSelectedOptions();

    std::vector<float> opacityScalars;
    opacityScalars.resize(spatialDataset->getNumPoints(), 0.0f);;
    if (celltypeClusterDataset.isValid())
    {
        const auto& clusters = celltypeClusterDataset->getClusters();
        for (const auto& cluster : clusters)
        {
            if (selectedClusterNamesList.contains(cluster.getName()))
            {
                const auto& indices = cluster.getIndices();
                for (const auto& index : indices)
                {
                    opacityScalars[index] = 1.0f; // set opacity to 1 for points in the cluster
                }
            }
        }
    }
    else
    {
        qDebug() << "No valid cell type cluster dataset selected for setting opacity scalars.";
    }

    //FIXME: this is hardcoded for now
    // if qc checked, set opacity to 0 for points that do not pass qc
    if (_settingsAction.getQcPassAction().isChecked())
    {
        // get qc pass dataset and set opacity to 0 for points that do not pass qc
        if (!_qcPassDataset.isValid())
        {
            for (const auto& data : mv::data().getAllDatasets())
            {
                if (data->getGuiName() == "qc_pass") // FIXME: hard-coded
                {
                    _qcPassDataset = data;
                    qDebug() << "Found qcPass dataset";
                    break;
                }
            }
        }

        if (_qcPassDataset.isValid())
        {
            const auto& clusters = _qcPassDataset->getClusters();
            for (const auto& cluster : clusters)
            {
                qDebug() << "QC Pass cluster name: " << cluster.getName();
                if (cluster.getName() == "id_0") //FIXME: hardcoded for now, need to make it more flexible in the future
                {
                    const auto& indices = cluster.getIndices();
                    for (const auto& index : indices)
                    {
                        opacityScalars[index] = 0.0f; // set opacity to 0 for points do not pass qc
                    }
                }
            }
        }
        else
        {
            qDebug() << "QC Pass dataset not found, cannot apply QC filter to opacity scalars.";
        }
        
    }

    _opacityDataset->setData(opacityScalars.data(), spatialDataset->getNumPoints(), 1);
    mv::events().notifyDatasetDataChanged(_opacityDataset);

    const QString opacityDatasetID = _opacityDataset.getDatasetId();

    const QString colorMap = _settingsAction.getColorMapSelectionAction().getColorMap();

    // TODO: remove
    //_computation.plotScatterplot(scatterplotNameForPC, positionDatasetID, colorDatasetID, celltypeClusterDatasetID, opacityDatasetID, colorMap);


    _computation.plotScatterplot(positionDatasetID, colorDatasetID, celltypeClusterDatasetID, opacityDatasetID, colorMap);


}

void ATACViewPlugin::addProjectAveragesSettingsGroupActions()
{
    const auto paPluginKind = "Project Averages";

    const auto paPluginFactory = mv::plugins().getPluginFactory(paPluginKind);

    // FIXME: temp hard code to only first two plugins
    int pluginCount = 0;

    for (const auto& paPlugin : mv::plugins().getPluginsByFactory(paPluginFactory))
    {
        pluginCount++;
        if (pluginCount > 2)
            break;

        auto analysisPlugin = dynamic_cast<AnalysisPlugin*>(paPlugin);

        auto groupAction = new GroupAction(this, analysisPlugin->getOutputDataset()->getGuiName());

        groupAction->addAction(analysisPlugin->getOutputDataset()->findChildByPath<DimensionPickerAction>("Settings/Averages Dataset Dimension"));

        groupAction->setExpanded(true);

        _groupForAction->addGroupAction(groupAction);
    }
}

void ATACViewPlugin::addOtherActions()
{

    GroupAction* groupActionPC = new GroupAction(this, "PC coloring settings");

    groupActionPC->addAction(&_settingsAction.getShowAdvancedSettingsAction());
    groupActionPC->addAction(&_settingsAction.getFeatureOptionAction());
    //groupActionPC->addAction(&_settingsAction.getFeatureDatasetAction());
    groupActionPC->addAction(&_settingsAction.getCellTypeDatasetAction());
    groupActionPC->addAction(&_settingsAction.getCellTypeSelectionAction());
    groupActionPC->addAction(&_settingsAction.getQcPassAction());
    groupActionPC->addAction(&_settingsAction.getColorMapSelectionAction());
    groupActionPC->addAction(&_settingsAction.getStartAnalysisAction());

    groupActionPC->setExpanded(true);

    GroupAction* groupActionPCAInputDimensions = new GroupAction(this, "PCA input dimensions");
    groupActionPCAInputDimensions->addAction(&_settingsAction.getDimensionSelectionAction());

    groupActionPC->setExpanded(true);

    _groupActionAdvanced = new GroupAction(this, "Advanced settings"); // settings that are not likely to be used by the user

    _groupActionAdvanced->addAction(&_settingsAction.getSpatialDatasetAction());
    _groupActionAdvanced->addAction(&_settingsAction.getSpatialClusterDatasetAction());
    _groupActionAdvanced->addAction(&_settingsAction.getATACAveragesDatasetAction());
    _groupActionAdvanced->addAction(&_settingsAction.getATACClusterDatasetAction());
    _groupActionAdvanced->addAction(&_settingsAction.getRNAAveragesDatasetAction());
    _groupActionAdvanced->addAction(&_settingsAction.getRNAClusterDatasetAction());
    //groupActionAdvanced->addAction(&_settingsAction.getScatterplotForPCAction()); TODO: remove, not needed now

    _groupActionAdvanced->setExpanded(false);

    _groupForAction->addGroupAction(groupActionPC);
    _groupForAction->addGroupAction(groupActionPCAInputDimensions);
    //_groupForAction->addGroupAction(_groupActionAdvanced);

    bool showAdvancedSettings = _settingsAction.getShowAdvancedSettingsAction().isChecked();

    if (showAdvancedSettings)
    {
        _groupForAction->addGroupAction(_groupActionAdvanced);
        //qDebug() << "addOtherActions(): advanced action is added";
    }
    //else
        //qDebug() << "addOtherActions(): advanced action is not added, showAdvancedSettings:" << showAdvancedSettings;
    // no need for else, this is called only when project is opened
}
// =============================================================================
// Serialization
// =============================================================================

void ATACViewPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    _settingsAction.fromVariantMap(variantMap["SettingsAction"].toMap());

        if (variantMap.contains("PCAInputDatasetID"))
        {
            QString pcaInputDatasetID = variantMap["PCAInputDatasetID"].toString();
            _pcaInputDataset = mv::data().getDataset<Points>(pcaInputDatasetID);
        }

        if (variantMap.contains("PCAOutputDatasetID"))
        {
            QString pcaOutputDatasetID = variantMap["PCAOutputDatasetID"].toString();
            _pcaOutputDataset = mv::data().getDataset<Points>(pcaOutputDatasetID);
        }

        if (variantMap.contains("ProjectionOutputDatasetID"))
        {
            QString projectionOutputDatasetID = variantMap["ProjectionOutputDatasetID"].toString();
            _projectionOutputDataset = mv::data().getDataset<Points>(projectionOutputDatasetID);
        }

        if (variantMap.contains("OpacityDatasetID"))
        {
            QString opacityDatasetID = variantMap["OpacityDatasetID"].toString();
            _opacityDataset = mv::data().getDataset<Points>(opacityDatasetID);
        }

}

QVariantMap ATACViewPlugin::toVariantMap() const
{
    QVariantMap variantMap = ViewPlugin::toVariantMap();

    _settingsAction.insertIntoVariantMap(variantMap);

    if (_pcaInputDataset.isValid())
        variantMap["PCAInputDatasetID"] = _pcaInputDataset.getDatasetId();

    if (_pcaOutputDataset.isValid())
        variantMap["PCAOutputDatasetID"] = _pcaOutputDataset.getDatasetId();

    if (_projectionOutputDataset.isValid())
        variantMap["ProjectionOutputDatasetID"] = _projectionOutputDataset.getDatasetId();

    if (_opacityDataset.isValid())
        variantMap["OpacityDatasetID"] = _opacityDataset.getDatasetId();

    return variantMap;
}

// =============================================================================
// Plugin Factory 
// =============================================================================

ATACViewPluginFactory::ATACViewPluginFactory()
{
    setIconByName("bullseye");
} 

ViewPlugin* ATACViewPluginFactory::produce()
{
    return new ATACViewPlugin(this);
}

mv::DataTypes ATACViewPluginFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    // This example analysis plugin is compatible with points datasets
    supportedTypes.append(PointType);

    return supportedTypes;
}

mv::gui::PluginTriggerActions ATACViewPluginFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this]() -> ATACViewPlugin* {
        return dynamic_cast<ATACViewPlugin*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (numberOfDatasets >= 1 && PluginFactory::areAllDatasetsOfTheSameType(datasets, PointType)) {
        auto pluginTriggerAction = new PluginTriggerAction(const_cast<ATACViewPluginFactory*>(this), this, "Example", "View example data", icon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (auto dataset : datasets)
                getPluginInstance();
        });

        pluginTriggerActions << pluginTriggerAction;
    }

    return pluginTriggerActions;
}
