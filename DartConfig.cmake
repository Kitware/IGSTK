# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "0:30:00 EDT")

# Dart server to submit results (used by client)
IF(DROP_METHOD MATCHES http)
  SET (DROP_SITE "public.kitware.com")
  SET (DROP_LOCATION "/cgi-bin/HTTPUploadDartFile.cgi")
ELSE(DROP_METHOD MATCHES http)
  SET (DROP_SITE "public.kitware.com")
  SET (DROP_LOCATION "/incoming")
  SET (DROP_SITE_USER "ftpuser")
  SET (DROP_SITE_PASSWORD "public")
ENDIF(DROP_METHOD MATCHES http)
SET (TRIGGER_SITE 
  "http://${DROP_SITE}/cgi-bin/Submit-IGSTK-TestingResults.pl")

# Project Home Page
#SET (PROJECT_URL "http://${DROP_SITE}/IFSTK")

# Dart server configuration 
SET (ROLLUP_URL "http://${DROP_SITE}/cgi-bin/IGSTK-rollup-dashboard.sh")
#SET (CVS_WEB_URL "http://${DROP_SITE}/cgi-bin/cvsweb.cgi/IGSTK/")
SET (CVS_WEB_CVSROOT "IGSTK")
SET (USE_DOXYGEN "On")
#SET (DOXYGEN_URL "http://${DROP_SITE}/IGSTK/index.html" )
SET (GNATS_WEB_URL "http://public.kitware.com/Bug/index.php")
SET (USE_GNATS "On")

# copy over the testing logo
#CONFIGURE_FILE(${PARAVIEW_SOURCE_DIR}/Web/Art/ParaViewLogo.gif ${PARAVIEW_BINARY_DIR}/Testing/HTML/TestingResults/Icons/Logo.gif COPYONLY)

