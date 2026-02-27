#pragma once

#include "PointData/PointData.h"
#include "ClusterData/ClusterData.h"

#include <AnalysisPlugin.h>

#include <actions/TriggerAction.h>
#include <actions/DatasetPickerAction.h>

#include <QEventLoop>

#include <vector>
#include <QDebug>


using namespace mv::gui;




class Computation
{
public:

    template<typename ActionType>
    ActionType* findActionByPath(mv::plugin::Plugin* plugin, const QString& path) {
        return dynamic_cast<ActionType*>(plugin->findChildByPath(path));
    }

    // - PC related computations -
    void triggerPCA(mv::Dataset<Points> inputDataset, mv::Dataset<Points> outputDataset);

    void triggerProjectAverages(mv::Dataset<Points> inputDataset, mv::Dataset<Points> outputDataset,
        mv::Dataset<Points> averagePointDataset, mv::Dataset<Clusters> averageClusterDataset,
        mv::Dataset<Clusters> positionClusterDataset);

    void plotScatterplot(const QString& scatterplotNameForPC, const QString& positionDatasetID,
        const QString& colorDatasetID, const QString& celltypeClusterDatasetID, const QString& opacityDatasetID);


private:

    mv::plugin::AnalysisPlugin* _pcaAnalysisPlugin = nullptr;

    mv::plugin::AnalysisPlugin* _projectAveragesPlugin = nullptr;
};
