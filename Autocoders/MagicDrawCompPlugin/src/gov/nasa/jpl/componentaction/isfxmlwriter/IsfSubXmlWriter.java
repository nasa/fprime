package gov.nasa.jpl.componentaction.isfxmlwriter;

import gov.nasa.jpl.componentaction.ISFPort;

import java.io.File;
import java.io.FileWriter;
import java.io.Writer;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.VelocityEngine;

import com.nomagic.magicdraw.core.Application;

public class IsfPortXmlWriter {
	/**
	 * Writes the Port XML files.
	 * @param port
	 * @param fileName
	 * @param outDir
	 * @param pluginDir
	 */
	public static void write(ISFPort port, String fileName, String outDir, File pluginDir) {
		//String tmplDir = "ISFXmlTemplates/";
		String templateFile = fileName + ".vm";
		String jarPath = "jar:file:" + pluginDir.toString() + "/FpCompAuto.jar";
		String outputFile = outDir + "/" + port.getName() + fileName + ".xml";
		// String outputFile = outDir + "/" + port.getNamespace() + port.getName() + fileName + ".xml";
		System.out.println("Writing to file: " + outputFile);
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
	        context.put("Port", port);
	        t.merge( context, writer );
	        writer.flush();
		} catch (Exception e) {
			System.out.println("===> Problem with getting resource:" + e.getMessage());
			Application.getInstance().getGUILog().log("===> Problem with getting resource:" + e.getMessage());
		}
	}
	
	
}
