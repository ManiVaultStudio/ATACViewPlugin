#pragma once

#include <ViewPlugin.h>

#include <Dataset.h>
#include <widgets/DropWidget.h>

#include <PointData/PointData.h>

#include <QWidget>

#include <actions/HorizontalToolbarAction.h>

#include "Actions/SettingsAction.h"
#include "Actions/DimensionSelectionAction.h"

#include "Computation.h"

/** All plugin related classes are in the ManiVault plugin namespace */
using namespace mv::plugin;

/** Drop widget used in this plugin is located in the ManiVault gui namespace */
using namespace mv::gui;

/** Dataset reference used in this plugin is located in the ManiVault util namespace */
using namespace mv::util;

namespace mv
{
    class CoreInterface;
}

class QLabel;


class ATACViewPlugin : public ViewPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param factory Pointer to the plugin factory
     */
    ATACViewPlugin(const PluginFactory* factory);

    /** Destructor */
    ~ATACViewPlugin() override = default;
    
    /** This function is called by the core after the view plugin has been created */
    void init() override;


    void computePCA();

    void projectPC();

    void plotPCProjection();

private:

    void addProjectAveragesSettingsGroupActions();

    void addOtherActions();

public: // Serialization
    /**
    * Load plugin from variant map
    * @param Variant map representation of the plugin
    */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
    * Save plugin to variant map
    * @return Variant map representation of the plugin
    */
    QVariantMap toVariantMap() const override;

private:
    Computation            _computation;


protected:
    mv::Dataset<Points>     _pcaInputDataset;                    /** Points smart pointer */
    mv::Dataset<Points>     _pcaOutputDataset;                      /** Points smart pointer */

    mv::Dataset<Points>     _projectionOutputDataset;

    mv::Dataset<Points>     _opacityDataset; // point opacity scalars fro selected cell types

    SettingsAction              _settingsAction;         

    GroupsAction*           groupForAction = nullptr;
    
};

/**
 * Example view plugin factory class
 *
 * Note: Factory does not need to be altered (merely responsible for generating new plugins when requested)
 */
class ATACViewPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.ATACViewPlugin"
                      FILE  "PluginInfo.json")

public:

    /** Default constructor */
    ATACViewPluginFactory();

    /** Creates an instance of the example view plugin */
    ViewPlugin* produce() override;

    /** Returns the data types that are supported by the example view plugin */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};
