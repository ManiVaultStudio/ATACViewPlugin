#include "DimensionSelectionAction.h"

DimensionSelectionAction::DimensionSelectionAction(QObject* parent) :
    GroupAction(parent, "DimensionSelectionAction"),
    _pickerAction(this, "DimensionPicker")
{
    setText("PCA Input Dimensions");
    setSerializationName("DimensionSelectionAction");

    setPopupSizeHint(QSize(400, 0));

    addAction(&_pickerAction);
}

void DimensionSelectionAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _pickerAction.fromParentVariantMap(variantMap);
}

QVariantMap DimensionSelectionAction::toVariantMap() const
{
    QVariantMap variantMap = GroupAction::toVariantMap();

    _pickerAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
