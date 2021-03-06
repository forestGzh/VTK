if(NOT CMAKE_C_COMPILER_ID STREQUAL "XL")
  if(NOT ${CMAKE_CROSSCOMPILING})
    set(vtkIOMovie_vtkoggtheora vtktheora)
  endif()
endif()
vtk_module(vtkIOMovie
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkTestingCore
    vtkImagingCore
    vtkImagingSources
  KIT
    vtkIO
  DEPENDS
    ${vtkIOMovie_vtkoggtheora}
    vtkogg
    vtkCommonExecutionModel
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonMisc
    vtkCommonSystem
  )
