PROJECT(SpatialObjects)
  
ADD_EXECUTABLE(SpatialObjectHierarchy SpatialObjectHierarchy.cxx)
TARGET_LINK_LIBRARIES(SpatialObjectHierarchy IGSTKSandbox)

ADD_EXECUTABLE(AxesObject AxesObject.cxx)
TARGET_LINK_LIBRARIES(AxesObject IGSTKSandbox)
  
ADD_EXECUTABLE(BoxObject BoxObject.cxx)
TARGET_LINK_LIBRARIES(BoxObject IGSTKSandbox)
  
ADD_EXECUTABLE(ConeObject ConeObject.cxx)
TARGET_LINK_LIBRARIES(ConeObject IGSTKSandbox)
  
ADD_EXECUTABLE(CylinderObject CylinderObject.cxx)
TARGET_LINK_LIBRARIES(CylinderObject IGSTKSandbox)
  
ADD_EXECUTABLE(EllipsoidObject EllipsoidObject.cxx)
TARGET_LINK_LIBRARIES(EllipsoidObject IGSTKSandbox)

ADD_EXECUTABLE(MeshObject MeshObject.cxx)
TARGET_LINK_LIBRARIES(MeshObject IGSTKSandbox)

ADD_EXECUTABLE(ImageObjects ImageObjects.cxx)
TARGET_LINK_LIBRARIES(ImageObjects IGSTKSandbox)

ADD_EXECUTABLE(TubeObject TubeObject.cxx)
TARGET_LINK_LIBRARIES(TubeObject IGSTKSandbox)

ADD_EXECUTABLE(VascularNetworkObject VascularNetworkObject.cxx)
TARGET_LINK_LIBRARIES(VascularNetworkObject IGSTKSandbox)

ADD_EXECUTABLE(ReadVascularNetworkObject ReadVascularNetworkObject.cxx)
TARGET_LINK_LIBRARIES(ReadVascularNetworkObject IGSTKSandbox)
