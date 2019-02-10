classes: \
  ./src/gov/nasa/jpl/componentaction/ISFComponent.java \
  ./src/gov/nasa/jpl/componentaction/IsfTopAction.java \
  ./src/gov/nasa/jpl/componentaction/ProcessISFTopology.java \
  ./src/gov/nasa/jpl/componentaction/MpmcsErrorStream.java \
  ./src/gov/nasa/jpl/componentaction/ISFPort.java \
  ./src/gov/nasa/jpl/componentaction/isfxmlwriter/IsfPortXmlWriter.java \
  ./src/gov/nasa/jpl/componentaction/isfxmlwriter/IsfSubXmlWriter.java \
  ./src/gov/nasa/jpl/componentaction/isfxmlwriter/IsfCompXmlWriter.java \
  ./src/gov/nasa/jpl/componentaction/IsfXmlAutocoderAction.java \
  ./src/gov/nasa/jpl/componentaction/LoadIDException.java \
  ./src/gov/nasa/jpl/componentaction/Utils.java \
  ./src/gov/nasa/jpl/componentaction/LoadIDConfigAction.java \
  ./src/gov/nasa/jpl/componentaction/IsfXmlAction.java \
  ./src/gov/nasa/jpl/componentaction/LoadIDConfig.java \
  ./src/gov/nasa/jpl/componentaction/Partition.java \
  ./src/gov/nasa/jpl/componentaction/IsfComponentAction.java \
  ./src/gov/nasa/jpl/componentaction/ProcessISFProject.java \
  ./src/gov/nasa/jpl/componentaction/ConnectorException.java \
  ./src/gov/nasa/jpl/componentaction/ISFSubsystem.java \
  ./src/gov/nasa/jpl/componentaction/IsfAbout.java \
  ./src/gov/nasa/jpl/componentaction/PortException.java \
  ./src/gov/nasa/jpl/componentaction/ComponentException.java \
  ./src/gov/nasa/jpl/componentaction/MainMenuConfigurator.java \
  ./src/gov/nasa/jpl/componentaction/TestISFExceptions.java
	mkdir -p $@
	javac -classpath lib/commons-cli-1.3.1.jar:lib/velocity-1.6.2-dep.jar:lib/md.jar:lib/bundles/com.nomagic.magicdraw.foundation_2.0.0.201803050637.jar:lib/bundles/org.eclipse.emf.common_2.10.1.v20140901-1043.jar:lib/bundles/javax.jmi_1.0.0.201803050637/javax_jmi-1_0-fr.jar:lib/bundles/org.junit_4.11.0.v201303080030/junit.jar:lib/bundles/com.nomagic.magicdraw.uml2_2.5.0.201803050637.jar:lib/bundles/org.eclipse.emf.ecore_2.10.1.v20140901-1043.jar:lib/md_api.jar: -d classes -encoding UTF8 $+
