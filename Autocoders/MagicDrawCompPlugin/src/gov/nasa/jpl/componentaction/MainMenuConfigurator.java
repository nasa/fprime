/*! \mainpage MagicDraw Component Plugin
 * 
 * \section intro Introduction
 * 
 * This is the documentation for the Magic Draw UML to XML generator plugin.
 *
 *
 * Currently, the program can create XML files from component models and subsystem models. Subsystems within subsystems along side multiple instances of subsystems and models are supported. 
 * Additionally, exceptions are thrown if there is something the program cannot handle and error messages are displayed onto the console. 
 * 
 * Notes
 * <ul>
 * 		<li>
 * 			If there are multiple subsystems in a model, make sure to create a stereotype "Top" and assign it to the "Top" level subsystem. This will ensure proper naming of the XML files. 
 * 			<ul>
 * 				<li>
 * 					A simple example of how to implement multiple subsystems and multiple instances of subsystems in a model can be found at tests/testMultipleSubsystems.mdzip.
 * 				</li>
 * 			</ul>
 * 		</li>
 * 		<li>
 * 			Please make sure to give instances of subsystems unique names.
 * 		</li>
 * 		<li>
 * 			Velocity (<a href = "http://velocity.apache.org/">an Apache template engine</a>) is being used to generate the XML files. These files can be found in the root of the MagicDrawPlugin folder and have .vm extensions.
 * 		</li>
 * </ul>
 * 
 * 
 * \section pluginUse Using The Plugin
 * 
 * <ul>
 * 		<li>
 * 			Load a project into MagicDraw.
 * 		</li>
 * 		<li>
 * 			Ensure that the project either only has zero or one subsystems, or has the "Top" stereotype applied to one of the subsystems if many exist.
 * 		</li>
 * 		<li>
 * 			Click on the "Component Autocoder" tab in the top toolbar. 
 * 		</li>
 * 		<li>
 * 			Click on "Auto Generate Component/Port/Topology XML".
 * 		</li>
 * 		<li>
 * 			Check the MagicDraw console to see if any errors were generated.
 * 		</li>
 * 		<li>
 * 			If not, the XML files can be found in <projectDir>/AutoXML/
 * 		</li>
 * </ul>
 * 
 * \section programFlow Program Flow
 * 
 * \subsection menuConfig Menu Configuration
 * 
 * These files take care of the menu configuration:
 * <br>
 * \ref MainMenuConfigurator.java - Creates "Component Autocoder" tab in the toolbar.
 * <br>
 * \ref IsfXMLAutocoderAction.java - Creates buttons within the "Component Autocoder" tab.
 * <br>
 * \ref  IsfAbout.java - Creates the modal element that displays the about information.
 * <br>
 * \ref IsfXmlAction.java - Action object that is called when "Auto Generate Component/Port/Topology XML" is clicked. Processes ports, components, and the entire topology. 
 * <br> 
 * \ref IsfCommandLine.java - Used to handle command line protocol.
 * <br>
 * \ref IsfTopAction.java - NOT USED.
 * <br>
 * \ref IsfComponentAction.java - NOT USED.
 * <br>
 * \ref LoadIDConfigAction.java - Creates button to load id configuration file within the "Component Autocoder" tab.
 * <br>
 * 
 * \subsection objectProcessing Processing
 * 
 * \ref ProcessISFProject.java - Object processes both components and ports at the same time.
 * 
 * <h3> Component </h3>
 * \ref ISFComponent.java - Helper object for component processing.
 * <br>
 * \ref IsfCompXmlWriter.java - Writes the processed components to XML documents.
 * 
 * 
 * <h3> Port </h3>
 * \ref ISFPort.java Helper object for port processing.
 * <br>
 * \ref IsfPortXmlWriter.java - Writes the processed ports to XML documents.
 * 
 * 
 * <h3> Topology </h3>
 * \ref ProcessISFTopology.java - This object processes the model to create the topology XML diagrams. It can handle multiple subsystem within subsystems as well and multiple instantiations of subsystems and components.
 * <br>
 * \ref ISFSubsystem.java - Contains many helper functions and objects used when process the model for the topology diagram.
 * <br>
 * \ref IsfSubXmlWriter.java - Writes the processed model to an XML document. 
 * 
 * 
 * \subsection exceptions Exceptions
 * These are the Exception objects of this plugin:
 * <br>
 * \ref ComponentException.java - Thrown during component processing.
 * <br>
 * \ref PortException.java - Thrown during port processing.
 * <br>
 * \ref ConnectorException.java - Thrown during the topology processing
 * <br>
 * \ref LoadIDException.java - Thrown during LoadIDConfig processing.
 * 
 * 
 * \subsection otherObjects Other
 * \ref Utils.java - Includes simple functions to help reduce code size (IE throwing ConnectorExceptions or raising warnings).
 * \ref LoadIDConfig.java - Loads id configuration file into the model and over writes component base and window ids with ones found in the file.
 *
 *\section aboutDoc About the Documentation
 *
 * More resources can be found in the base MagicDrawCompPlugin/doc/ or at MagicDrawCompPlugin/README.txt
 * <br>
 * To update the documentation to reflect code changes:
 * <ul>
 * 		<li>
 * 			This Introduction area can be edited in the top comment block in MagicDrawCompPlugin/src/gov/nasa/jpl/componentaction/MainMenuConfigurator.java.
 * 		</li>
 * 		<li>
 * 			Make sure Doxygen and Graphviz are both installed.
 * 		</li>
 * 		<li>
 * 			Open the Doxygen Gui.
 * 		</li>
 * 		<li>
 * 			Under the "File" tab at the top, click "Open" and select the file titled Doxyfile in the MagicDrawCompPlugin directory.
 * 		</li>
 * 		<li>
 * 			Modify the settings (IE version number) in the GUI.
 * 		</li>
 * 		<li>
 * 			To regenerate the documentation files, click on the "Run" tab in the GUI and click "Run doxygen".
 * 		</li>
 * 		<li>
 * 			To update the PDF found in the doc folder, go to doc/rtf and convert refman.rtf to refman.doc using Microsoft Word, and then convert the .doc file to .pdf.
 * 		</li>
 * </ul>
 *
 * \section contact Contact
 * 
 * Until the plugin is updated even further, I will be able to answer questions any time. 
 * My name is Saikiran Ramanan and I worked on the plugin as an intern during the Summer of 2016. 
 * My email address is saikiranra@gmail.com
 * 
 */
package gov.nasa.jpl.componentaction;

import com.nomagic.actions.AMConfigurator;
import com.nomagic.actions.ActionsCategory;
import com.nomagic.actions.ActionsManager;
import com.nomagic.actions.NMAction;
import com.nomagic.magicdraw.actions.MDActionsCategory;

/**
 * Class for configuring main menu and adding  new submenu.
 * @version $Date: 2007-10-05 09:51:43 +0300 (Fri, 05 Oct 2007) $ $Revision: 52410 $
 * @author Donatas Simkunas
 */
public class MainMenuConfigurator implements AMConfigurator
{

	String MPMCS="Component Autocoder";

	/**
	 * Action will be added to manager.
	 */
	private NMAction action;

	/**
	 * Creates configurator.
	 * @param action action to be added to main menu.
	 */
	public MainMenuConfigurator(NMAction action)
	{
		this.action = action;
	}

	/**
	 * @see com.nomagic.actions.AMConfigurator#configure(ActionsManager)
	 *  Methods adds action to given manager Examples category.
	 */
	public void configure(ActionsManager mngr)
	{
		// searching for Examples action category
		ActionsCategory category = (ActionsCategory) mngr.getActionFor(MPMCS);

		if( category == null )
		{
			// creating new category
			category = new MDActionsCategory(MPMCS,MPMCS);
			category.setNested(true);
			mngr.addCategory(category);
		}
		category.addAction(action);
		
	}
	
	public int getPriority()
	{
		return AMConfigurator.MEDIUM_PRIORITY;
	}

}