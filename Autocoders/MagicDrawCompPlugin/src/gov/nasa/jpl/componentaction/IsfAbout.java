package gov.nasa.jpl.componentaction;

import java.awt.event.ActionEvent;
import java.io.File;
import java.io.IOException;
import java.util.Date;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import com.nomagic.magicdraw.actions.MDAction;
import com.nomagic.magicdraw.core.Application;
/**
 * Action class for information regarding the plugin
 */
public class IsfAbout extends MDAction {
	File pluginDir;
	private static final long serialVersionUID = -6790954285526957354L;

	public IsfAbout(String id, String name, File pluginDir)
    {
        super(id, name, null, null);
        this.pluginDir = pluginDir;
    }
	
	/**
	 * Used to display content in the "About" tab. Edit this for tab changes. 
	 */
    public void actionPerformed(ActionEvent e)
    {
		try {
			JarFile jf = new JarFile(pluginDir + "/FpCompAuto.jar");
			ZipEntry manifest = jf.getEntry("META-INF/MANIFEST.MF");
			long manifestTime = manifest.getTime();
			Date testDate = new Date(manifestTime);
			
			File xmlFile = new File(pluginDir + "/plugin.xml");
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(xmlFile);
			doc.getDocumentElement().normalize();
			
			NodeList nList = doc.getElementsByTagName("plugin");
			Node node = nList.item(0);
			Element el = (Element)node;
			
			String name = el.getAttribute("name");
			String provider = el.getAttribute("provider-name");
			String version = el.getAttribute("version");
			String date = testDate.toString();
			String copyright = "Copyright:  2016 California Institute of Technology. All rights reserved.";
			
			jf.close();
			
			Application.getInstance().getGUILog().showMessage(name
					+ "\n" + provider
					+ "\nVersion " + version
					+ "\nPlugin Generation Date: " + date
					+ "\n" + copyright);
		} catch(IOException io) {
			Application.getInstance().getGUILog().showMessage(io.getMessage());
		} catch (Exception e1) {
			e1.printStackTrace();
		}
    }

}
