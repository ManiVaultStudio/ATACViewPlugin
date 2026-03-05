#include "Computation.h"



using namespace mv;
using namespace mv::gui;

void Computation::triggerPCA(mv::Dataset<Points> inputDataset, mv::Dataset<Points> outputDataset)
{
    /*if (!_pcaAnalysisPlugin) {
        qDebug() << "Requesting PCA Analysis plugin from ManiVault...";
        _pcaAnalysisPlugin = mv::plugins().requestPlugin<AnalysisPlugin>("PCA Analysis", { inputDataset }, { outputDataset });
    }*/

    //if (!_pcaAnalysisPlugin) {
    //    qCritical() << "PCA Analysis Plugin not found in ManiVault!";
    //    return;
    //}

    // check if a PCA plugin is attached to the output dataset, otherwise request a new one
    auto checkAction = dynamic_cast<IntegralAction*>(outputDataset->findChildByPath("PCA/Number of PCA components"));
    if (!checkAction) {
        qDebug() << "Number of PCA components action not found, request a PCA plugin";
        _pcaAnalysisPlugin = mv::plugins().requestPlugin<AnalysisPlugin>("PCA Analysis", { inputDataset }, { outputDataset });
    }

    // get the related actions

    auto componentsAction = dynamic_cast<IntegralAction*>(outputDataset->findChildByPath("PCA/Number of PCA components"));
    if (!componentsAction) {
        qCritical() << "Number of PCA components action not found";
    }
    componentsAction->setMaximum(inputDataset->getNumDimensions());
    componentsAction->setValue(inputDataset->getNumDimensions());


    auto startAction = dynamic_cast<TriggerAction*>(outputDataset->findChildByPath("PCA/Start analysis"));

    if (startAction) {
        startAction->trigger();
        qDebug() << "PCA analysis started";
       /* QEventLoop loop;
        loop.processEvents();
        outputDataset->setSelectionIndices({});
        mv::events().notifyDatasetDataSelectionChanged(outputDataset);*/

    }
    else {
        qCritical() << "Start analysis action not found in PCA dataset";

    }
}

void Computation::triggerProjectAverages(
    mv::Dataset<Points> inputDataset, mv::Dataset<Points> outputDataset,
    mv::Dataset<Points> averagePointDataset, mv::Dataset<Clusters> averageClusterDataset,
    mv::Dataset<Clusters> positionClusterDataset)
{
    //if (!_projectAveragesPlugin) {
    //    _projectAveragesPlugin = mv::plugins().requestPlugin<AnalysisPlugin>("Project Averages", { inputDataset }, { outputDataset });
    //}

    //if (!_projectAveragesPlugin) {
    //    qCritical() << "Project Average Plugin not found in ManiVault!";
    //    return;
    //}

    auto checkAction = dynamic_cast<DatasetPickerAction*>(outputDataset->findChildByPath("Settings/Averages Point Dataset"));
    if (!checkAction) {
        qDebug() << "Action not found, request a Project Averages plugin";
        _projectAveragesPlugin = mv::plugins().requestPlugin<AnalysisPlugin>("Project Averages", { inputDataset }, { outputDataset });
    }
    
    auto averagePointDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(outputDataset->findChildByPath("Settings/Averages Point Dataset"));
    if (!averagePointDatasetPickerAction) {
        qCritical() << "Averages Point Dataset picker action not found in projection output dataset";
        return;
    }
    averagePointDatasetPickerAction->setCurrentDataset(averagePointDataset);

    auto averageClusterDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(outputDataset->findChildByPath("Settings/Averages Cluster Dataset"));
    if (!averageClusterDatasetPickerAction) {
        qCritical() << "Averages Cluster Dataset picker action not found in projection output dataset";
        return;
    }
    averageClusterDatasetPickerAction->setCurrentDataset(averageClusterDataset);

    auto positionClusterDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(outputDataset->findChildByPath("Settings/Position Cluster Dataset"));
    if (!positionClusterDatasetPickerAction) {
        qCritical() << "Position Cluster Dataset picker action not found in projection output dataset";
        return;
    }
    positionClusterDatasetPickerAction->setCurrentDataset(positionClusterDataset);

    auto startAction = dynamic_cast<TriggerAction*>(outputDataset->findChildByPath("Settings/Update Trigger Action"));
    if (startAction) {
        startAction->trigger();
        qDebug() << "Project averages analysis started";
       /* QEventLoop loop;
        loop.processEvents();
        outputDataset->setSelectionIndices({});
        mv::events().notifyDatasetDataSelectionChanged(outputDataset);*/
    }
    else {
        qCritical() << "Start analysis action not found in projection output dataset";
    }
    
}

// TODO: remove
//void Computation::plotScatterplot(const QString& scatterplotNameForPC, const QString& positionDatasetID, const QString& colorDatasetID, const QString& celltypeClusterDatasetID, const QString& opacityDatasetID, const QString& colorMap)
//{
//    auto scatterplotViewFactory = mv::plugins().getPluginFactory("Scatterplot View");
//
//    if (scatterplotViewFactory)
//    {
//        bool pluginfound = false;
//        for (auto plugin : mv::plugins().getPluginsByFactory(scatterplotViewFactory))
//        {
//            if (plugin->getGuiName() == scatterplotNameForPC)
//            {
//                pluginfound = true;
//
//                // set the position dataset
//                mv::gui::DatasetPickerAction* pointDatasetPickerValueAction = findActionByPath<DatasetPickerAction>(plugin, "Settings/Datasets/Position");
//                if (pointDatasetPickerValueAction)
//                {
//                    //if (!pointDatasetPickerValueAction->getCurrentDataset().isValid())
//                    {
//                        auto embeddingDataset = mv::data().getDataset(positionDatasetID);
//                        if (embeddingDataset.isValid())
//                        {
//                            pointDatasetPickerValueAction->setCurrentDataset(embeddingDataset);
//                        }
//                    }
//                }
//                else
//                {
//                    qWarning() << "Position dataset picker action not found in scatterplot plugin";
//                }
//
//                // set coloring
//                mv::gui::DatasetPickerAction* coloringDatasetPickerValueAction = findActionByPath<DatasetPickerAction>(plugin, "Settings/Datasets/Color");
//                if (coloringDatasetPickerValueAction)
//                {
//                    {
//                        auto coloringDataset = mv::data().getDataset(colorDatasetID);
//                        if (coloringDataset.isValid())
//                        {
//                            coloringDatasetPickerValueAction->setCurrentDataset(coloringDataset);
//                        }
//                    }
//                }
//                else
//                {
//                    qWarning() << "Coloring dataset picker action not found in scatterplot plugin";
//                }
//
//                // set color map same as in the figure
//                mv::gui::OptionAction* coloringColorMapOptionsAction = findActionByPath<OptionAction>(plugin, "Settings/Coloring/1D Color map/Current color map");
//                if (coloringColorMapOptionsAction)
//                {
//                    //coloringColorMapOptionsAction->setCurrentText("Viridis");
//                    coloringColorMapOptionsAction->setCurrentText("colorMap");
//                }
//                else
//                {
//                    qWarning() << "Coloring color map option action not found in scatterplot plugin";
//                }
//
//
//                // set opacity to 0 for non-target cell types
//                
//                mv::gui::DatasetPickerAction* opacityDatasetPickerValueAction = findActionByPath<DatasetPickerAction>(plugin, "Settings/Plot/Point/Point opacity/Source dataset");
//                //if (opacityPickerValueAction && opacityDatasetPickerValueAction)
//                if (opacityDatasetPickerValueAction)
//                {
//                    auto opacityDataset = mv::data().getDataset(opacityDatasetID);
//                    opacityDatasetPickerValueAction->setCurrentDataset(opacityDataset);
//                }
//                else {
//                    qWarning() << "opacityDatasetPickerValueAction not found";
//                }
//
//                break;
//            }
//
//        }
//        if (!pluginfound)
//        {
//            qCritical() << "Scatterplot plugin not found with name:" << scatterplotNameForPC;
//        }
//    }
//}

void Computation::plotScatterplot(const QString& positionDatasetID, const QString& colorDatasetID, const QString& celltypeClusterDatasetID, const QString& opacityDatasetID, const QString& colorMap)
{
    const QString scatterplotNameForPC = "Scatterplot PC";

    auto scatterplotViewFactory = mv::plugins().getPluginFactory("Scatterplot View");

    if (scatterplotViewFactory)
    {
        bool pluginfound = false;
        for (auto plugin : mv::plugins().getPluginsByFactory(scatterplotViewFactory))
        {
            if (plugin->getGuiName() == scatterplotNameForPC)
            {
                pluginfound = true;

                _pcScatterplotViewPlugin = dynamic_cast<ViewPlugin*>(plugin);

                setupPCAScatterplot(positionDatasetID, colorDatasetID, celltypeClusterDatasetID, opacityDatasetID, colorMap);

                break;
            }

        }
        if (!pluginfound)
        {
            //qCritical() << "Scatterplot plugin not found with name:" << scatterplotNameForPC;
            qDebug() << "Scatterplot plugin not found with name:" << scatterplotNameForPC << ", opening a new one";
            //_pcScatterplotViewPlugin = mv::plugins().requestPlugin<ViewPlugin>("Scatterplot View");
            _pcScatterplotViewPlugin = mv::plugins().requestViewPlugin("Scatterplot View");
             //mv::plugins().requestPlugin("Scatterplot View");
            if (_pcScatterplotViewPlugin)
            {
                qDebug() << "Scatterplot plugin opened with name:" << _pcScatterplotViewPlugin->getGuiName();
                _pcScatterplotViewPlugin->getGuiNameAction().setString(scatterplotNameForPC);
                qDebug() << "Scatterplot plugin GUI name set to:" << _pcScatterplotViewPlugin->getGuiName();

                setupPCAScatterplot(positionDatasetID, colorDatasetID, celltypeClusterDatasetID, opacityDatasetID, colorMap);
            }
        }
    }
}

void Computation::setupPCAScatterplot(const QString& positionDatasetID,
    const QString& colorDatasetID, const QString& celltypeClusterDatasetID,
    const QString& opacityDatasetID, const QString& colorMap)
{
    if (!_pcScatterplotViewPlugin) {
        qCritical() << "Scatterplot PC is not set up";
        return;
    }

    // set the position dataset
    mv::gui::DatasetPickerAction* pointDatasetPickerValueAction = findActionByPath<DatasetPickerAction>(_pcScatterplotViewPlugin, "Settings/Datasets/Position");
    if (pointDatasetPickerValueAction)
    {
        //if (!pointDatasetPickerValueAction->getCurrentDataset().isValid())
        {
            auto embeddingDataset = mv::data().getDataset(positionDatasetID);
            if (embeddingDataset.isValid())
            {
                pointDatasetPickerValueAction->setCurrentDataset(embeddingDataset);
            }
        }
    }
    else
    {
        qWarning() << "Position dataset picker action not found in scatterplot plugin";
    }

    // set coloring
    mv::gui::DatasetPickerAction* coloringDatasetPickerValueAction = findActionByPath<DatasetPickerAction>(_pcScatterplotViewPlugin, "Settings/Datasets/Color");
    if (coloringDatasetPickerValueAction)
    {
        {
            auto coloringDataset = mv::data().getDataset(colorDatasetID);
            if (coloringDataset.isValid())
            {
                coloringDatasetPickerValueAction->setCurrentDataset(coloringDataset);
            }
        }
    }
    else
    {
        qWarning() << "Coloring dataset picker action not found in scatterplot plugin";
    }

    // set color map same as in the figure
    mv::gui::OptionAction* coloringColorMapOptionsAction = findActionByPath<OptionAction>(_pcScatterplotViewPlugin, "Settings/Coloring/1D Color map/Current color map");
    if (coloringColorMapOptionsAction)
    {
        //coloringColorMapOptionsAction->setCurrentText("Viridis");
        coloringColorMapOptionsAction->setCurrentText(colorMap);
    }
    else
    {
        qWarning() << "Coloring color map option action not found in scatterplot plugin";
    }


    // set opacity to 0 for non-target cell types

    mv::gui::DatasetPickerAction* opacityDatasetPickerValueAction = findActionByPath<DatasetPickerAction>(_pcScatterplotViewPlugin, "Settings/Plot/Point/Point opacity/Source dataset");
    //if (opacityPickerValueAction && opacityDatasetPickerValueAction)
    if (opacityDatasetPickerValueAction)
    {
        auto opacityDataset = mv::data().getDataset(opacityDatasetID);
        opacityDatasetPickerValueAction->setCurrentDataset(opacityDataset);
    }
    else {
        qWarning() << "opacityDatasetPickerValueAction not found";
    }
}