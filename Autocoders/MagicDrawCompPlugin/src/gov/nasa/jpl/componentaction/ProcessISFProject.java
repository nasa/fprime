package gov.nasa.jpl.componentaction;

import gov.nasa.jpl.componentaction.isfxmlwriter.IsfCompXmlWriter;
import gov.nasa.jpl.componentaction.isfxmlwriter.IsfPortXmlWriter;

import java.io.File;
import java.io.IOException;
import java.util.*;

import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;
import com.nomagic.uml2.ext.jmi.helpers.StereotypesHelper;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Element;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.InstanceSpecification;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.NamedElement;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.Operation;
import com.nomagic.uml2.ext.magicdraw.classes.mdkernel.TypedElement;
import com.nomagic.uml2.ext.magicdraw.mdprofiles.Stereotype;

/**
 * This singleton class will process an ISF Project.
 * 
 * @author Garth Watney
 */
public class ProcessISFProject {
	private static Project project;
	private static List<ISFComponent> componentList;
	private static List<ISFPort> portList;

	private ProcessISFProject() {
	}

	/**
	 * Main function to process the project.
	 * 
	 * <p>
	 * <ul>
	 * 		<li>
	 * 			Creates a directory to output XML files into if it does not already exist.
	 * 		</li>
	 * 		<li>
	 * 			Processes all the components in the model and generates an XML file for each of them.
	 * 		</li>
	 * 		<li>
	 * 			Processes all the ports in the model and generates an XML file to store all the port data.
	 * 		</li>
	 * </ul>
	 * 
	 * 
	 * @param proj Project object
	 * @param pluginDir Folder to create XML folder in
	 * @throws IOException
	 */
	public static void process(Project proj, File pluginDir) throws IOException {
		project = proj;
		componentList = new ArrayList<ISFComponent>();
		portList = new ArrayList<ISFPort>();

		Application.getInstance().getGUILog().clearLog();
		if (project == null) {
			System.out.println("***No model found");
			Application.getInstance().getGUILog().log("***No model found");
		} else {

			List<String> compStereotypes = Arrays.asList("active", "passive",
					"queued");

			System.out.println("Project Name = " + project.getName());
			Application.getInstance().getGUILog()
			.log("My Project Name = " + project.getName());

			File projectFile = new File(project.getFileName());
			String projectDir = projectFile.getParent() + "/AutoXML";
			
			File dir = new File(projectDir);
			if(!dir.exists()) {
				dir.mkdir();
			}

			// Parse through the model for each component stereotype
			Application.getInstance().getGUILog()
			.log("Parsing model for Components");
			try {
				for (String compType : compStereotypes) {
					List<Element> compElementList = getElementsOfStereotype(compType);
					if(compType == "passive"){
						compElementList.addAll(getElementsOfStereotype("Passive Component"));
					}
					else if(compType == "active"){
						compElementList.addAll(getElementsOfStereotype("Active Component"));
					}
					else if(compType == "queued"){
						compElementList.addAll(getElementsOfStereotype("Queued Component"));
					}
					processComponents(compElementList, compType);
				}


				// Print the components
				for (ISFComponent c : componentList) {
					c.print();
				}

				// Or Ports can have the Stereotype of "PortType"
				Application.getInstance().getGUILog()
				.log("Parsing model for Ports");
				List<Element> newPortElementList = getElementsOfStereotype("PortType");
				processPorts(newPortElementList);

				// Print out the Port meta model
				for (ISFPort p : portList) {
					p.print();
				}
				
				// Write out an XML Interface for each Component in the meta model
				for (ISFComponent c : componentList) {
					IsfCompXmlWriter.write(c, "ComponentAi", projectDir, pluginDir);
				}
				// Write out an XML Interface for each Port in the meta model
				for (ISFPort p : portList) {
					IsfPortXmlWriter.write(p, "PortAi", projectDir, pluginDir);
				}
			}

			catch(PortException e) {
				Application.getInstance().getGUILog()
				.log("<font color=\"#FF0000\">===> FATAL Exception: " + e.getMessage() + "</font>");
				throw new RuntimeException(e);
			}
		}
	}
	
	/**
	 * Function to process the project, except restricts component search to the processPackage.
	 * 
	 * <p>
	 * <ul>
	 * 		<li>
	 * 			Creates a directory to output XML files into if it does not already exist.
	 * 		</li>
	 * 		<li>
	 * 			Processes all the components in the processPackage and generates an XML file for each of them.
	 * 		</li>
	 * 		<li>
	 * 			Processes all the ports in the processPackage and generates an XML file to store all the port data.
	 * 		</li>
	 * </ul>
	 * 
	 * 
	 * @param proj Project object
	 * @param pluginDir Folder to create XML folder in
	 * @param processPackage String of the process package
	 * @throws IOException
	 */
	public static void process(Project proj, File pluginDir, String processPackage) throws IOException {
		project = proj;
		componentList = new ArrayList<ISFComponent>();
		portList = new ArrayList<ISFPort>();
		Application.getInstance().getGUILog().clearLog();
		List<String> compStereotypes = Arrays.asList("active", "passive",
				"queued");
		File projectFile = new File(project.getFileName());
		String projectDir = projectFile.getParent() + "/AutoXML";

		File dir = new File(projectDir);
		if(!dir.exists()) {
			dir.mkdir();
		}

		try {
			for (String compType : compStereotypes) {
				List<Element> compElementList = getElementsOfStereotypeAndPackage(compType, processPackage);
				processComponents(compElementList, compType);
			}

			// Or Ports can have the Stereotype of "PortType"
			List<Element> newPortElementList = getElementsOfStereotypeAndPackage("PortType", processPackage);
			processPorts(newPortElementList);

			// Write out an XML Interface for each Component in the meta model
			for (ISFComponent c : componentList) {
				IsfCompXmlWriter.write(c, "ComponentAi", projectDir, pluginDir);
			}
			// Write out an XML Interface for each Port in the meta model
			for (ISFPort p : portList) {
				IsfPortXmlWriter.write(p, "PortAi", projectDir, pluginDir);
			}
		}

		catch(PortException e) {
			Application.getInstance().getGUILog()
			.log("<font color=\"#FF0000\">===> FATAL Exception: " + e.getMessage() + "</font>");
			throw new RuntimeException(e.getMessage());
		}
	}

	/**
	 * Converts ports to ISFPort objects with associated data and return elements and adds it to the portList.
	 * @param portElementList List of ports
	 * @throws PortException
	 */
	private static void processPorts(List<Element> portElementList) throws PortException {

		for (Element port : portElementList) {
			if (!(port instanceof InstanceSpecification)) {
				ISFPort thisPort = new ISFPort(port);
				portList.add(thisPort);

				Collection<Element> ownedElements = port.getOwnedElement();
				for (Element data : ownedElements) {

					if (!(data instanceof InstanceSpecification)) {
						// An argument in the Port
						if (data instanceof TypedElement) {
							//protect against missing type
							if(((TypedElement) data).getType() == null) {
								String errstr = ((TypedElement)data).getName() 
										+ " of port " 
										+ ((NamedElement)port).getName() 
										+  " has no data type";
								System.out.println(errstr);
								throw new PortException(errstr);
							}
							thisPort.addData(data);
						}
						// A return value in the Port
						if (data instanceof Operation) {
							thisPort.addReturn(data);
						}
					}
				}
			}
			else {
				System.out.println("No ports found");
				Application.getInstance().getGUILog().log("No ports found");
			}
		}
	}

	/**
	 * Creates ISFComponent objects for all components with associated ports and adds them toe the componentList.
	 * @param compElementList
	 * @param stereotype
	 * @throws PortException
	 */
	private static void processComponents(List<Element> compElementList,
			String stereotype) throws PortException {
		List<String> portTypeNames = Arrays.asList("sync_input", "async_input",
				"guarded_input", "Async" , "Sync" , "Input" , "Guarded" , "Output" , "output", "input");
		List<String> portRoleNames = Arrays.asList("Cmd", "Telemetry", "CmdRegistration", 
				"CmdResponse", "LogEvent", "LogTextEvent", "ParamGet", "ParamSet", "TimeGet");
		for (Element el : compElementList) {
			ISFComponent thisComp = new ISFComponent(el);
			componentList.add(thisComp);
			thisComp.setStereoType(stereotype);
			Collection<Element> ownedElements = el.getOwnedElement();
			// Loop through all the Port elements in this component
			for (Element oe : ownedElements) {
				//System.out.println(oe.getHumanName() + ": " + StereotypesHelper.hasStereotypeOrDerived(oe, "input"));
				String portStereotype = thisComp.getPortStereotype(oe);
				// Check the Port against all the ISF Port Stereotypes
				if (portTypeNames.contains(portStereotype)) {
					thisComp.newAddPort(oe);														
				} else if(portRoleNames.contains(portStereotype)) {
					thisComp.newRolePort(oe);
				} else if (!portStereotype.equals("Instance")) {
					Application
					.getInstance()
					.getGUILog()
					.log("*** Warning: Port Stereotype = "
							+ portStereotype + ":  In Component "
							+ thisComp.getName());
					System.out.println("==> Warning: Port Stereotype = "
							+ portStereotype + ":  In Component "
							+ thisComp.getName());
				}
			}
			try {
				thisComp.checkComponent();
				
			}
			catch(ComponentException e) {
				Application.getInstance().getGUILog()
				.log("<font color=\"#FF0000\">===> FATAL Exception: " + e.getMessage() + "</font>");
				throw new RuntimeException(e.getMessage());
			}
		}
	}

	/**
	 * Returns a list of elements with specified input stereotype string.
	 * @param stereotype stereotype string to look for
	 * @return list of Elements
	 */
	private static List<Element> getElementsOfStereotype(String stereotype) {
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
	
	/**
	 * Returns a list of elements with specified input stereotype string and within the processPackage. 
	 * @param stereotype stereotype string to look for
	 * @param processPackage package string to look for
	 * @return list of Elements
	 */
	private static List<Element> getElementsOfStereotypeAndPackage(String stereotype, String processPackage) {
		Stereotype st;
		List<Element> elementList = new ArrayList<Element>();

		st = StereotypesHelper.getStereotype(project, stereotype);
		if (st != null) {
			elementList = StereotypesHelper.getExtendedElements(st);
			if (elementList.isEmpty())
				System.out.println("Stereotype: " + stereotype
						+ ", list is EMPTY?");
			else {
				for(int i = elementList.size()-1; i>=0; i--) {
					Element comp = elementList.get(i);
					if(comp instanceof NamedElement) {
						NamedElement namedComp = (NamedElement)comp;
						if(!processPackage.equals(namedComp.getQualifiedName().split("::")[0])) {
							elementList.remove(i);
						}
					}
				}
			}
		} else {
			System.out.println("==> Warning no stereotype:" + stereotype);
			Application.getInstance().getGUILog()
			.log("==> Warning no stereotype:" + stereotype);
		}
		return elementList;
	}

}
