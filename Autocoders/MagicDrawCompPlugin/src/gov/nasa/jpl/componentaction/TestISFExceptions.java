package gov.nasa.jpl.componentaction;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;


import junit.framework.Test;
import junit.framework.TestSuite;

import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;
import com.nomagic.magicdraw.tests.MagicDrawTestCase;
import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.mdprofiles.Stereotype;

/**
 * JUnit test suite to check that exception handling works.
 * 
 * Most tests involve three parts:
 * 
 * <ul>
 * 		<li>
 * 			Load the Magic Draw file.
 * 		</li>
 * 		<li>
 * 			Run the plugin function that will be tested (IE, process from ProcessISFTopology will only test the processing of the topology models, and so on).
 * 		</li>
 * 		<li>
 * 			Check if an exception is thrown with the correct error message, or if a pass test is being run, check if the appropriate files have been generated. 
 * 		</li>
 * <ul>
 * 
 *
 */
public class TestISFExceptions extends MagicDrawTestCase {
	private String pluginDir;
	
	public static void main (String[] args) {
		junit.textui.TestRunner.run(suite());
	}
	
	public TestISFExceptions(String testMethodToRun, String testName) {
		super(testMethodToRun, testName);
		pluginDir = System.getProperty("user.dir")+ "/tests";//System.getenv("PLUGINDIR");
	}
	
	public TestISFExceptions(String testMethodToRun) {
		super(testMethodToRun);
		pluginDir = System.getProperty("user.dir")+ "/tests";//System.getenv("PLUGINDIR");
	}
	
	@Override
	protected void setUpTest() throws Exception {
		super.setUpTest();
		setSkipMemoryTest(true);
	}
	
	
	public void testComponentNoConnections() throws IOException {
		openProject("tests/testComponentNoConnections.mdzip");
		Project proj = Application.getInstance().getProject();
		ProcessISFTopology.process(proj, new File(pluginDir));
		String log = Application.getInstance().getGUILog().getLoggedMessages();
		assertTrue(log.contains("has no connections"));
	}
	
	/**
	 * Checks if ports without types throw exceptions
	 * @throws IOException
	 */
	public void testPortException() throws IOException {
		openProject("tests/testPortException.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFProject.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("has no data type"));
			return;
		}
		fail("PortException expected");
	}
	
	/**
	 * Checks if connections with types that do not match throw exceptions
	 * @throws IOException
	 */
	public void testMismatchedType() throws IOException {
		openProject("tests/testMismatchedType.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFTopology.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("different data types"));
			return;
		}
		fail("ConnectorException expected");
	}
	
	/**
	 * Checks if connections with directions that do not match throw exceptions
	 * @throws IOException
	 */
	public void testMismatchedDirections() throws IOException {
		openProject("tests/testMismatchedDirections.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFTopology.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("invalid"));
			return;
		}
		fail("ConnectorException expected");
	}
	
	/**
	 * Checks if connections originating from the same output port throws exceptions
	 * @throws IOException
	 */
	public void testDoubleMultiplicity() throws IOException {
		openProject("tests/testDoubleMultiplicity.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFTopology.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("same multiplicity"));
			return;
		}
		fail("ConnectorException expected");
	}
	
	/**
	 * Checks if passive components with async inputs throw exceptions
	 * @throws IOException
	 */
	public void testPassiveComponent() throws IOException {
		openProject("tests/testPassiveComponent.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFProject.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("Passive component"));
			return;
		}
		fail("ComponentException expected");
	}
	
	/**
	 * @precondition assumes that there is exactly one component in the project
	 * @param projectName
	 * @param compType
	 * @return
	 */
	public ISFComponent getCompForTesting(String projectName, String compType){
		openProject(projectName);
		Project proj = Application.getInstance().getProject();
		List<Element> compList = getElementsOfStereotype(compType, proj);
		assertTrue(!compList.isEmpty());
		Element el = compList.get(0);
		List<String> portTypeNames = Arrays.asList("sync_input", "async_input",
				"guarded_input", "output", "input");
		
		ISFComponent thisComp = new ISFComponent(el);
		thisComp.setStereoType(compType);
		Collection<Element> owned = el.getOwnedElement();
		for(Element oe : owned) {
			String portStereotype = thisComp.getPortStereotype(oe);
			if (portTypeNames.contains(portStereotype)) {
				try {
					thisComp.newAddPort(oe);
				} catch(PortException pe) {
					fail(pe.getMessage());
				}
			}
		}
		return thisComp;
	}
	
	/**
	 * Checks if active component without async ports gives warnings
	 * @throws IOException
	 */
	public void testActiveComponent() {
		ISFComponent thisComp = getCompForTesting("tests/testActiveComponent.mdzip", "active");
		assertFalse(thisComp.isValid("active", "async_input", true));
	}
	
	/**
	 * Checks if queued component without async ports gives warnings
	 * @throws IOException
	 */
	public void testQueuedComponentAsync() throws IOException {
		ISFComponent thisComp = getCompForTesting("tests/testQueuedComponentAsync.mdzip", "queued");
		assertFalse(thisComp.isValid("queued", "async_input", true));
	}
	
	/**
	 * Checks if queued component without sync ports gives warnings
	 * @throws IOException
	 */
	public void testQueuedComponentSync() throws IOException {
		ISFComponent thisComp = getCompForTesting("tests/testQueuedComponentSync.mdzip", "queued");
		assertFalse(thisComp.isValid("queued", "sync_input", true));
	}
	
	/**
	 * Checks if topology is generated correctly
	 * @throws IOException
	 */
	public void testProcessTopology() throws IOException {
		openProject("tests/testGenerateTopology.mdzip");
		Project proj = Application.getInstance().getProject();
		ProcessISFTopology.process(proj, new File(pluginDir));
		File generated = new File(pluginDir + "/AutoXML/top1TopologyAppAi.xml");
		assertTrue(generated.exists());
	}
	
	public void testAbout() {
		IsfAbout about = new IsfAbout(null,"About", new File(pluginDir));
		about.actionPerformed(null);
	}
	
	public void testProcessProject() throws IOException {
		openProject("tests/testAllCorrect.mdzip");
		Project proj = Application.getInstance().getProject();
		ProcessISFProject.process(proj, new File(pluginDir));
		ProcessISFTopology.process(proj, new File(pluginDir));
	}
	
	public void testNoSource() throws IOException {
		openProject("tests/testNoSource.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFTopology.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("top1 has an end with a null role"));
			return;
		}
		fail("ComponentException expected");
	}
	
	public void testNoTarget() throws IOException {
		openProject("tests/testNoTarget.mdzip");
		Project proj = Application.getInstance().getProject();
		try {
			ProcessISFTopology.process(proj, new File(pluginDir));
		}
		catch(RuntimeException e) {
			assertTrue(e.getMessage().contains("top1 has an end with a null role"));
			return;
		}
		fail("ComponentException expected");
	}
	
	/**
	 * Tests both subsystems within subsystems, as well as multiple instances of one subsystem.
	 * 
	 * Note: The input file fails parsing for ports and components.
	 * @throws IOException
	 */
	public void testMultipleSubsystems() throws IOException {
		openProject("tests/testMultipleSubsystems.mdzip");
		Project proj = Application.getInstance().getProject();
		ProcessISFTopology.process(proj, new File(pluginDir));
		File generated = new File(pluginDir + "/AutoXML/IBD2TopologyAppAi.xml");
		assertTrue(generated.exists());
	}
	
	
	
	public void testCommandLine() {
		
	}
	
	@Override
	protected void tearDownTest() throws Exception {
		super.tearDownTest();
		closeAllProjects();
	}
	
	/**
	 * Organizes the order in which tests run.
	 * @return
	 */
	public static Test suite() {
		TestSuite suite = new TestSuite();
		suite.addTest(new TestISFExceptions("testMultipleSubsystems"));
		suite.addTest(new TestISFExceptions("testPortException"));
		suite.addTest(new TestISFExceptions("testMismatchedType"));
		suite.addTest(new TestISFExceptions("testMismatchedDirections"));
		suite.addTest(new TestISFExceptions("testDoubleMultiplicity"));
		suite.addTest(new TestISFExceptions("testPassiveComponent"));
		suite.addTest(new TestISFExceptions("testActiveComponent"));
		suite.addTest(new TestISFExceptions("testQueuedComponentAsync"));
		suite.addTest(new TestISFExceptions("testQueuedComponentSync"));
		suite.addTest(new TestISFExceptions("testProcessTopology"));
		suite.addTest(new TestISFExceptions("testComponentNoConnections"));
		suite.addTest(new TestISFExceptions("testAbout"));
		suite.addTest(new TestISFExceptions("testProcessProject"));
		
		
		suite.addTest(new TestISFExceptions("testNoSource"));
		suite.addTest(new TestISFExceptions("testNoTarget"));
		
		return suite;
	}
	
	private static List<Element> getElementsOfStereotype(String stereotype, Project project) {
		Stereotype st;
		List<Element> elementList = new ArrayList<Element>();

		st = StereotypesHelper.getStereotype(project, stereotype);
		if (st != null) {
			elementList = StereotypesHelper.getExtendedElements(st);
			if (elementList.isEmpty())
				System.out.println("Stereotype: " + stereotype
						+ ", list is EMPTY?");
		} else {
			System.out.println("==> Warning no stereotype:" + stereotype);
			Application.getInstance().getGUILog()
			.log("==> Warning no stereotype:" + stereotype);
		}
		return elementList;
	}

}
