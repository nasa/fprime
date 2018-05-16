package gov.nasa.jpl.componentaction.isfxmlwriter;

//import gov.nasa.jpl.componentaction.ISFComponent;
import gov.nasa.jpl.componentaction.ISFSubsystem;

import java.io.File;
import java.io.FileWriter;
import java.io.Writer;
import java.util.Arrays;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.VelocityEngine;
//import org.apache.velocity.runtime.resource.loader.JarResourceLoader;
import com.nomagic.magicdraw.core.Application;

public class IsfSubXmlWriter {
	/**
	 * Writes the subsystem topology XML file.
	 * @param top
	 * @param deployment
	 * @param fileName
	 * @param outDir
	 * @param pluginDir
	 */
	public static void write(ISFSubsystem.topologyModel top, String deployment, String fileName, String outDir, File pluginDir) {
		//String tmplDir = "ISFXmlTemplates/";
		String templateFile = "TopologyAi.vm";
		String outputFile = outDir + "/" + fileName + deployment + "AppAi.xml";
		System.out.println("Writing to file: " + outputFile);
		String jarPath = "jar:file:" + pluginDir.toString() + "/FpCompAuto.jar";
		//Application.getInstance().getGUILog().log("File: " + outputFile);
		Application.getInstance().getGUILog()
		.log("Writing new file: " + outputFile);
		
		try {
			Writer writer = new FileWriter( outputFile );
			VelocityEngine ve = new VelocityEngine();
			ve.setProperty("resource.loader", "file, jar");
			ve.setProperty("file.resource.loader.class", "org.apache.velocity.runtime.resource.loader.FileResourceLoader");
			ve.setProperty("file.resource.loader.path", "./," + pluginDir.toString());

			ve.setProperty("jar.resource.loader.class", "org.apache.velocity.runtime.resource.loader.JarResourceLoader");
			ve.setProperty("jar.resource.loader.path", jarPath);
			ve.init();
			/*  next, get the Template  */
			Template t = ve.getTemplate( templateFile );
			/*  create a context and add data */
			VelocityContext context = new VelocityContext();
			context.put("topology", top);
			context.put("deployment", deployment);
			context.put("arrays", Arrays.class);
			t.merge( context, writer );
			writer.flush();
		} 
		catch (Exception e) {
			System.out.println("===> Problem with getting Plugin Resource:" + e.getMessage());
			Application.getInstance().getGUILog().log("===> Problem with getting Plugin Resource: " + e.getMessage());
		}
	}
	
}