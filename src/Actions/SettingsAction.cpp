#include "SettingsAction.h"

#include "src/ATACViewPlugin.h"



#include <QHBoxLayout>

using namespace mv::gui;

SettingsAction::SettingsAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _atacViewPlugin(dynamic_cast<ATACViewPlugin*>(parent)),
    _spatialDatasetAction(this, "Spatial dataset"),
    _spatialClusterDatasetAction(this, "Spatial cluster dataset"),
    _averagesClusterDatasetAction(this, "ATAC cluster dataset"),
    _scatterplotForPCAction(this, "Scatterplot for PC"),

    _featureDatasetAction(this, "Feature dataset"),
    _cellTypeDatasetAction(this, "Cell type dataset"),
    _cellTypeSelectionAction(this, "Cell type selection"),
    _dimensionSelectionAction(this),
    _qcPassAction(this, "QC pass"),
    _startAnalysisAction(this, "Start analysis")
{
    setText("Settings");
    setSerializationName("SettingsAction");
    setPopupSizeHint(QSize(300, 0));
    
    setShowLabels(true);
    setLabelSizingType(LabelSizingType::Auto);



    if (_atacViewPlugin == nullptr)
        return;

    setupDatasetPickerActions(_atacViewPlugin);
    setupscatterplotForPCAction();

    _dimensionSelectionAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::File);
    //_dimensionSelectionAction.setDefaultWidgetFlags(OptionsAction::File);
    
    addAction(&_spatialDatasetAction);
    addAction(&_spatialClusterDatasetAction);
    addAction(&_averagesClusterDatasetAction);
    addAction(&_scatterplotForPCAction);

    addAction(&_featureDatasetAction);
    addAction(&_cellTypeDatasetAction);
    addAction(&_cellTypeSelectionAction);
    addAction(&_dimensionSelectionAction);
    addAction(&_startAnalysisAction);

    addAction(&_qcPassAction);

    //_spatialClusterDatasetAction.setToolTip()

    //connect(&_spatialDatasetAction, &DatasetPickerAction::currentIndexChanged, this, [this]() {
    //    qDebug() << "Selected spatial dataset changed in settings action";
    //    //auto selectedDataset = _spatialDatasetAction.getCurrentDataset<Points>();
    //    });

    connect(&_featureDatasetAction, &DatasetPickerAction::currentIndexChanged, this, [this]() {
        //qDebug() << "Selected dataset changed in settings action";
        auto selectedDataset = _featureDatasetAction.getCurrentDataset<Points>();
        _dimensionSelectionAction.getPickerAction().setPointsDataset(selectedDataset);
        });

    connect(&_cellTypeDatasetAction, &DatasetPickerAction::currentIndexChanged, this, [this]() {

        //qDebug() << "Selected cell type dataset changed in settings action";

        setupCellTypeSelectionAction();
        });

    //connect(&_cellTypeSelectionAction, &OptionsAction::selectedOptionsChanged, this, [this]() {
    //    qDebug() << "Selected cell type(s) changed in settings action";

    //    // TODO: change the view or only change the view when start analysis is triggered?
    //    });


    connect(&_startAnalysisAction, &TriggerAction::triggered, this, [this]() {
        //qDebug() << "Start analysis triggered in settings action";
        _atacViewPlugin->computePCA();
        });

   
}

void SettingsAction::setupDatasetPickerActions(ATACViewPlugin* atacViewPlugin)
{

    _spatialDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == PointType;
        });

    _spatialClusterDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == ClusterType;
        });

    _averagesClusterDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == ClusterType;
        });

    _featureDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == PointType;
        });

    _cellTypeDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == ClusterType;
        });
}

void SettingsAction::setupCellTypeSelectionAction()
{
    auto cellTypeDataset = _cellTypeDatasetAction.getCurrentDataset<Clusters>();

    QStringList cellTypeOptions;
    if (cellTypeDataset.isValid()) {
        auto clusters = cellTypeDataset->getClusters();

        for (const auto& cluster : clusters)
            cellTypeOptions.append(cluster.getName());
    }
    _cellTypeSelectionAction.setOptions(cellTypeOptions);
}

void SettingsAction::setupscatterplotForPCAction()
{
    QStringList scatterplotNames = {};

    auto scatterplotViewFactory = mv::plugins().getPluginFactory("Scatterplot View");
    if (scatterplotViewFactory) {
        for (auto plugin : mv::plugins().getPluginsByFactory(scatterplotViewFactory)) {
            scatterplotNames.append(plugin->getGuiName());
        }
    }

    _scatterplotForPCAction.setOptions(scatterplotNames);

}

void SettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _spatialDatasetAction.fromParentVariantMap(variantMap);
    _spatialClusterDatasetAction.fromParentVariantMap(variantMap);
    _averagesClusterDatasetAction.fromParentVariantMap(variantMap);
    _scatterplotForPCAction.fromParentVariantMap(variantMap);

    _featureDatasetAction.fromParentVariantMap(variantMap);
    _cellTypeDatasetAction.fromParentVariantMap(variantMap);
    _cellTypeSelectionAction.fromParentVariantMap(variantMap);
    _dimensionSelectionAction.fromParentVariantMap(variantMap);

    _qcPassAction.fromParentVariantMap(variantMap);
    
}

QVariantMap SettingsAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _spatialDatasetAction.insertIntoVariantMap(variantMap);
    _spatialClusterDatasetAction.insertIntoVariantMap(variantMap);
    _averagesClusterDatasetAction.insertIntoVariantMap(variantMap);
    _scatterplotForPCAction.insertIntoVariantMap(variantMap);

    _featureDatasetAction.insertIntoVariantMap(variantMap);
    _cellTypeDatasetAction.insertIntoVariantMap(variantMap);
    _cellTypeSelectionAction.insertIntoVariantMap(variantMap);
    _dimensionSelectionAction.insertIntoVariantMap(variantMap);

    _qcPassAction.insertIntoVariantMap(variantMap);


    return variantMap;
}

