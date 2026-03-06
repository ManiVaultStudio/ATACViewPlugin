#include "SettingsAction.h"

#include "src/ATACViewPlugin.h"



#include <QHBoxLayout>

using namespace mv::gui;

SettingsAction::SettingsAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _atacViewPlugin(dynamic_cast<ATACViewPlugin*>(parent)),
    _spatialDatasetAction(this, "Spatial dataset"),
    _spatialClusterDatasetAction(this, "Spatial cluster dataset"),
    _atacClusterDatasetAction(this, "ATAC cluster dataset"),
    _rnaClusterDatasetAction(this, "RNA cluster dataset"),
    _atacAveragesDatasetAction(this, "ATAC averages dataset"),
    _rnaAveragesDatasetAction(this, "RNA averages dataset"),
    //_scatterplotForPCAction(this, "Scatterplot for PC"),// TODO: remove
    _featureOptionAction(this, "Feature for PCA"),
    _showAdvancedSettingsAction(this, "Show advanced settings"),
    _cellTypeDatasetAction(this, "Cell type dataset"),
    _cellTypeSelectionAction(this, "Cell type"),
    _dimensionSelectionAction(this),
    _qcPassAction(this, "QC pass"),
    _colorMapSelectionAction(this, "Color map"),
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
    //setupscatterplotForPCAction();//TODO: remove

    _dimensionSelectionAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::File);
    //_dimensionSelectionAction.setDefaultWidgetFlags(OptionsAction::File);

    _featureOptionAction.initialize(QStringList({ "ATAC", "RNA" }), "ATAC");
    
    addAction(&_spatialDatasetAction);
    addAction(&_spatialClusterDatasetAction);
    addAction(&_atacClusterDatasetAction);
    addAction(&_rnaClusterDatasetAction);
    addAction(&_atacAveragesDatasetAction);
    addAction(&_rnaAveragesDatasetAction);
    //addAction(&_scatterplotForPCAction);// TODO: remove

    addAction(&_featureOptionAction);
    addAction(&_cellTypeDatasetAction);
    addAction(&_cellTypeSelectionAction);
    addAction(&_dimensionSelectionAction);
    addAction(&_startAnalysisAction);

    addAction(&_qcPassAction);
    addAction(&_colorMapSelectionAction);
    addAction(&_showAdvancedSettingsAction);

    _spatialDatasetAction.setToolTip("Spatial map");
    _spatialClusterDatasetAction.setToolTip("Cluster annotations for spatial cells used as the projection base");
    _atacClusterDatasetAction.setToolTip("Cluster annotations for ATAC data used to project cluster averages");
    _rnaClusterDatasetAction.setToolTip("Cluster annotations for RNA data used to project cluster averages");
    _atacAveragesDatasetAction.setToolTip("Cluster-by-peak matrix");
    _rnaAveragesDatasetAction.setToolTip("Cluster-by-gene matrix");
    _cellTypeDatasetAction.setToolTip("Cell type annotations");
    _cellTypeSelectionAction.setToolTip("Select cell types to display"); 
    _qcPassAction.setToolTip("Show QC-passed cells only");
    _colorMapSelectionAction.setToolTip("Color map for PC");
    _showAdvancedSettingsAction.setToolTip("Show advanced settings");

    //connect(&_spatialDatasetAction, &DatasetPickerAction::currentIndexChanged, this, [this]() {
    //    qDebug() << "Selected spatial dataset changed in settings action";
    //    //auto selectedDataset = _spatialDatasetAction.getCurrentDataset<Points>();
    //    });

    connect(&_featureOptionAction, &OptionAction::currentTextChanged, this, [this]() {
        qDebug() << "Feature option changes";

        if (_featureOptionAction.getCurrentText() == "ATAC")
        {
            auto featureDataset = _atacAveragesDatasetAction.getCurrentDataset<Points>();

            if (!featureDataset.isValid())
            {
                qDebug() << "No valid feature dataset for ATAC";
                return;
            }

            _dimensionSelectionAction.getPickerAction().setPointsDataset(featureDataset);
            qDebug() << "SettingsAction dimensionSelectionAction dataset set to " << featureDataset->getGuiName();
        }
        else if (_featureOptionAction.getCurrentText() == "RNA")
        {
            auto featureDataset = _rnaAveragesDatasetAction.getCurrentDataset<Points>();

            if (!featureDataset.isValid())
            {
                qDebug() << "No valid feature dataset for RNA";
                return;
            }

            _dimensionSelectionAction.getPickerAction().setPointsDataset(featureDataset);
            qDebug() << "SettingsAction dimensionSelectionAction dataset set to " << featureDataset->getGuiName();
        }
        });

    //connect(&_featureDatasetAction, &DatasetPickerAction::currentIndexChanged, this, [this]() {
    //    //qDebug() << "Selected dataset changed in settings action";
    //    auto selectedDataset = _featureDatasetAction.getCurrentDataset<Points>();
    //    _dimensionSelectionAction.getPickerAction().setPointsDataset(selectedDataset);
    //    });

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

    _atacClusterDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == ClusterType;
        });

    _rnaClusterDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == ClusterType;
        });

    _atacAveragesDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == PointType;
        });

    _rnaAveragesDatasetAction.setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
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

//void SettingsAction::setupscatterplotForPCAction()
//{
// // TODO: remove
//    QStringList scatterplotNames = {};
//
//    auto scatterplotViewFactory = mv::plugins().getPluginFactory("Scatterplot View");
//    if (scatterplotViewFactory) {
//        for (auto plugin : mv::plugins().getPluginsByFactory(scatterplotViewFactory)) {
//            scatterplotNames.append(plugin->getGuiName());
//        }
//    }
//
//    _scatterplotForPCAction.setOptions(scatterplotNames);
//
//}

void SettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _spatialDatasetAction.fromParentVariantMap(variantMap);
    _spatialClusterDatasetAction.fromParentVariantMap(variantMap);
    _atacClusterDatasetAction.fromParentVariantMap(variantMap);

    //_scatterplotForPCAction.fromParentVariantMap(variantMap);// TODO: remove

    
    _cellTypeDatasetAction.fromParentVariantMap(variantMap);
    _cellTypeSelectionAction.fromParentVariantMap(variantMap);
    _dimensionSelectionAction.fromParentVariantMap(variantMap);

    _qcPassAction.fromParentVariantMap(variantMap);
    _colorMapSelectionAction.fromParentVariantMap(variantMap);

    _showAdvancedSettingsAction.fromParentVariantMap(variantMap);
    
}

QVariantMap SettingsAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _spatialDatasetAction.insertIntoVariantMap(variantMap);
    _spatialClusterDatasetAction.insertIntoVariantMap(variantMap);
    _atacClusterDatasetAction.insertIntoVariantMap(variantMap);
    //_scatterplotForPCAction.insertIntoVariantMap(variantMap);// TODO: remove

   
    _cellTypeDatasetAction.insertIntoVariantMap(variantMap);
    _cellTypeSelectionAction.insertIntoVariantMap(variantMap);
    _dimensionSelectionAction.insertIntoVariantMap(variantMap);

    _qcPassAction.insertIntoVariantMap(variantMap);
    _colorMapSelectionAction.insertIntoVariantMap(variantMap);

    _showAdvancedSettingsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

