ZCL_FILE=third_party/connectedhomeip/src/app/zap-templates/zcl/zcl.json
GEN_TEMPLATE_FILE=third_party/connectedhomeip/src/app/zap-templates/app-templates.json

zap -i data_model/lighting-app.zap -z $ZCL_FILE -g $GEN_TEMPLATE_FILE