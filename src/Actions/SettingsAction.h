#pragma once

#include <actions/GroupAction.h>
#include <actions/TriggerAction.h>
#include <actions/DatasetPickerAction.h>
#include <actions/OptionsAction.h>
//#include <actions/OptionAction.h>
#include <actions/ToggleAction.h>
#include<actions/ColorMap1DAction.h>

#include <PointData/DimensionPickerAction.h>

#include <ClusterData/ClusterData.h>
#include <ColorData/ColorData.h>

#include "DimensionSelectionAction.h"


using namespace mv::gui;

class ATACViewPlugin;
/**
 * Settings action class
 *
 * Action class for configuring settings
 */
class SettingsAction : public GroupAction
{
public:
    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE SettingsAction(QObject* parent, const QString& title);

    ///**
    // * Get action context menu
    // * @return Pointer to menu
    // */
    //QMenu* getContextMenu();

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */

    QVariantMap toVariantMap() const override;

public: // Action getters

    // - PC related settings -
    // not likely to be defined by user   
    DatasetPickerAction& getSpatialDatasetAction() { return _spatialDatasetAction; }
    DatasetPickerAction& getSpatialClusterDatasetAction() { return _spatialClusterDatasetAction; }
    DatasetPickerAction& getAveragesClusterDatasetAction() { return _averagesClusterDatasetAction; }
    //OptionAction& getScatterplotForPCAction() { return _scatterplotForPCAction; }// TODO: remove

    // need to be defined by user
    DatasetPickerAction& getFeatureDatasetAction() { return _featureDatasetAction; }
    DatasetPickerAction& getCellTypeDatasetAction() { return _cellTypeDatasetAction; }
    OptionsAction& getCellTypeSelectionAction() { return _cellTypeSelectionAction; }
    TriggerAction& getStartAnalysisAction() { return _startAnalysisAction; }
    DimensionSelectionAction& getDimensionSelectionAction() { return _dimensionSelectionAction; }

    ToggleAction& getQcPassAction() { return _qcPassAction; }

    ColorMap1DAction& getColorMapSelectionAction() { return _colorMapSelectionAction; }


protected:
    void setupDatasetPickerActions(ATACViewPlugin* atacViewPlugin);

    void setupCellTypeSelectionAction();

public:

    //void setupscatterplotForPCAction();//TODO: remove

private:
    ATACViewPlugin* _atacViewPlugin;

    //- PC related settings -
    // not likely to be defined by user
    DatasetPickerAction                 _spatialDatasetAction; // obsm/spatial_cirro_grid_4col
    DatasetPickerAction                 _spatialClusterDatasetAction; // Cluster_v4_label
    DatasetPickerAction                 _averagesClusterDatasetAction; // Clusters (for ATAC averages) or cell_type (for RNA)
    //OptionAction                        _scatterplotForPCAction;// scatterplot View for a spatial map colored by PC projection // TODO: remove


    // need to be defined by user
    DatasetPickerAction                 _featureDatasetAction; // e.g. avgExprDataset (for ATAC) or marm_Cluster_v4_metacell (for RNA)
    DatasetPickerAction                 _cellTypeDatasetAction; // e.g. Cluster_NT
    // select a cell type from the above cell type dataset
    OptionsAction                       _cellTypeSelectionAction; //e.g. Glut or GABA   
    TriggerAction                       _startAnalysisAction;
    DimensionSelectionAction            _dimensionSelectionAction;
    ColorMap1DAction                    _colorMapSelectionAction;


    ToggleAction                        _qcPassAction;

};
