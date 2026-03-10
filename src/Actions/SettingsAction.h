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

    // advanced settings: not likely to be used by user
    DatasetPickerAction& getSpatialDatasetAction() { return _spatialDatasetAction; }
    DatasetPickerAction& getSpatialClusterDatasetAction() { return _spatialClusterDatasetAction; }
    DatasetPickerAction& getATACClusterDatasetAction() { return _atacClusterDatasetAction; }
    DatasetPickerAction& getRNAClusterDatasetAction() { return _rnaClusterDatasetAction; }
    DatasetPickerAction& getATACAveragesDatasetAction() { return _atacAveragesDatasetAction; }
    DatasetPickerAction& getRNAAveragesDatasetAction() { return _rnaAveragesDatasetAction; }
    DimensionPickerAction& getPCSelectionAction() { return _pcSelectionAction; }
    TriggerAction& getExportImputedATACAction() { return _exportImputedATACAction; }
    TriggerAction& getExportImputedRNAAction() { return _exportImputedRNAAction; }

    // PC coloring
    ToggleAction& getShowAdvancedSettingsAction() { return _showAdvancedSettingsAction; }
    OptionAction& getFeatureOptionAction() { return _featureOptionAction; }
    DatasetPickerAction& getCellTypeDatasetAction() { return _cellTypeDatasetAction; }
    OptionsAction& getCellTypeSelectionAction() { return _cellTypeSelectionAction; }
    ColorMap1DAction& getColorMapSelectionAction() { return _colorMapSelectionAction; }
    ToggleAction& getQcPassAction() { return _qcPassAction; }
    TriggerAction& getStartAnalysisAction() { return _startAnalysisAction; }

    DimensionSelectionAction& getDimensionSelectionAction() { return _dimensionSelectionAction; }


protected:
    void setupDatasetPickerActions(ATACViewPlugin* atacViewPlugin);

    void setupCellTypeSelectionAction();

    void setUpDimensionSelectionAction();

private:
    ATACViewPlugin* _atacViewPlugin;

    // advanced settings: not likely to be used by user
    DatasetPickerAction                 _spatialDatasetAction; // obsm/spatial_cirro_grid_4col
    DatasetPickerAction                 _spatialClusterDatasetAction; // Cluster_v4_label
    DatasetPickerAction                 _atacClusterDatasetAction; // Clusters (for ATAC averages)
    DatasetPickerAction                 _rnaClusterDatasetAction;// cell_type (for RNA)
    DatasetPickerAction                 _atacAveragesDatasetAction; // avgExprDataset (for ATAC)
    DatasetPickerAction                 _rnaAveragesDatasetAction; // marm
    DimensionPickerAction               _pcSelectionAction; // which pc to display, e.g. PC1
    TriggerAction                       _exportImputedATACAction;
    TriggerAction                       _exportImputedRNAAction;

    // PC coloring 
    OptionAction                        _featureOptionAction;
    DatasetPickerAction                 _cellTypeDatasetAction; // e.g. Cluster_NT    
    OptionsAction                       _cellTypeSelectionAction; //e.g. Glut or GABA; select a cell type from the above cell type dataset 
    TriggerAction                       _startAnalysisAction;   
    ColorMap1DAction                    _colorMapSelectionAction;
    ToggleAction                        _qcPassAction;
    ToggleAction _showAdvancedSettingsAction; // whether to show advanced settings

    DimensionSelectionAction            _dimensionSelectionAction;


};
