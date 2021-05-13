package gov.nasa.jpl.componentaction;

import java.io.File;
import java.io.IOException;

import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;

import com.nomagic.magicdraw.commandline.CommandLine;
import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;
import com.nomagic.magicdraw.core.project.ProjectDescriptorsFactory;

/**
 * Class to run MagicDraw plugin from command line
 * 
 * Before running on headless environment, use Xvfb to set up a virtual X11 display server
 * <code>/usr/bin/Xvfb :1 -screen 0 1024x768x24
 * export DISPLAY=:1 </code>
 * 
 * To run the utility, use the script isfpluginexec.sh in the MagicDrawCompPlugin directory
 */
public class IsfCommandLine extends CommandLine
{
	static File pluginDir;
	static String projectFile;
	private static final String PROJECT = "project_name";
	private static final String PACKAGE = "package";
	//private static final String USAGE = "java -cp <MagicDraw libraries> -Dinstall.root=<MagicDraw installation directory> -Xmx900M -Xss2M -XX:PermSize=40M -XX:MaxPermSize=150M gov.nasa.jpl.componentaction.IsfCommandLine [options]";
	private static final String USAGE = "isfpluginexec [options]\nBefore running on headless environment, use Xvfb to set up a virtual X11 display server\n/usr/bin/Xvfb :1 -screen 0 1024x768x24\nexport DISPLAY=:1";
	private static final String COPYRIGHT = "Copyright 2015 (California Institute of Technology)\nALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.";
	/**
	 * Reads the command line arguments and launches the program
	 *
	 * @param args application arguments.
	 */
	public static void main(String[] args) {
		try {
			Options opt = new Options();
			opt.addOption("h", false, "Print help for this plugin");
			opt.addOption(PROJECT, true, "The project to process");
			opt.addOption(PACKAGE, true, "The package to generate the xml for");
			opt.addOption("verbose", false, "Run project in verbose mode");
			DefaultParser parser = new DefaultParser();
			org.apache.commons.cli.CommandLine cl = null;
			cl = parser.parse(opt, args);
			if(cl.hasOption("h")) {
				HelpFormatter f = new HelpFormatter();
				f.printHelp(100, USAGE, COPYRIGHT, opt , "");
				System.exit(0);
			} else {
				if(cl.hasOption(PROJECT)) {
					System.setProperty(PROJECT, cl.getOptionValue(PROJECT));
				} else {
					System.out.println("No project specified!");
					HelpFormatter f = new HelpFormatter();
					f.printHelp(100, USAGE, COPYRIGHT, opt , "");
					System.exit(1);
				}
				if(cl.hasOption(PACKAGE)) {
					System.setProperty(PACKAGE, cl.getOptionValue(PACKAGE));
				}
			}
		} catch (ParseException e) {
			e.printStackTrace();
		}
		projectFile = System.getProperty(PROJECT, "");
		if (projectFile.length() == 0)
		{
			System.out.println("Project file not defined!");
			return;
		}

		System.out.println("launching");
		new IsfCommandLine().launch(args);
	}

	/**
	 * Runs autocoder on the project specified
	 */
	@Override
	protected byte execute() {
		System.out.println("executing");
		Application application = Application.getInstance();
		pluginDir = new File(System.getenv("BUILD_ROOT") + "/Autocoders/MagicDrawCompPlugin/");
		application.getProjectsManager()
		.loadProject(ProjectDescriptorsFactory.createProjectDescriptor(new File(projectFile).toURI()), true);
		Project project = application.getProject();

		String processPackage = System.getProperty(PACKAGE);

		try {
			if(processPackage!=null) {
				ProcessISFProject.process(project, pluginDir, processPackage);
			}
			else {
				ProcessISFProject.process(project, pluginDir);
			}
			System.out.println("*** Completed Component/Port Processing");
			ProcessISFTopology.process(project, pluginDir);
			System.out.println("*** Completed Topology processing");
		} catch (IOException e1) {
			e1.printStackTrace();
			System.out.println("**** " + e1.getMessage());
		} catch(RuntimeException e) {
			System.out.println(e.getMessage());
			throw e;
		}
		return 0;
	}
}
