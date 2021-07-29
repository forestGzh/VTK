vtk_module(vtkRenderingOpenGL2
  IMPLEMENTS
    vtkRenderingCore
  BACKEND
    OpenGL2
  IMPLEMENTATION_REQUIRED_BY_BACKEND
  TEST_DEPENDS
    vtkIOLegacy
    vtkRenderingImage
    vtkRenderingFreeType
    vtkTestingCore
    vtkTestingRendering
    vtkImagingGeneral
    vtkImagingSources
    vtkInteractionStyle
    vtkIOPLY
  KIT
    vtkOpenGL
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkRenderingCore
  PRIVATE_DEPENDS
    vtkCommonExecutionModel
    vtkCommonMath
    vtkCommonSystem
    vtkCommonTransforms
    vtkglew
    vtksys
  )