set(Module_SRCS vtkCityGMLReader.cxx)
vtk_module_library(vtkIOCityGML ${Module_SRCS})

# pugixml does not set _INCLUDE_DIRS or _LIBRARIES
if(VTK_USE_SYSTEM_PUGIXML AND NOT pugixml_INCLUDE_DIRS)
  find_package(pugixml REQUIRED)
  vtk_module_link_libraries(vtkIOCityGML LINK_PUBLIC pugixml)
endif()
