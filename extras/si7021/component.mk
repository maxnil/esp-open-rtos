# Component makefile for extras/si7021

# expected anyone using si7021 driver includes it as 'si7021/si7021.h'
INC_DIRS += $(si7021_ROOT)..

# args for passing into compile rule generation
si7021_SRC_DIR =  $(si7021_ROOT)

$(eval $(call component_compile_rules,si7021))
