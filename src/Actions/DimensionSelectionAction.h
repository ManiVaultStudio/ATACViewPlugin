#pragma once

#include <actions/GroupAction.h>

#include <PointData/DimensionsPickerAction.h>

/**
 * Dimension selection action class
 *
 * Action class for point data dimension selection
 *
 * @author Thomas Kroes
 */
class DimensionSelectionAction : public GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    DimensionSelectionAction(QObject* parent);

public: // Action getters

    DimensionsPickerAction& getPickerAction() { return _pickerAction; };

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
    DimensionsPickerAction  _pickerAction;    /** Dimension picker action */
};