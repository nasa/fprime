package gov.nasa.jpl.componentaction;


import java.awt.event.ActionEvent;
import java.io.File;
import java.io.IOException;
import com.nomagic.magicdraw.actions.MDAction;
import com.nomagic.magicdraw.core.Application;
import com.nomagic.magicdraw.core.Project;

/**
 * Used to generate component, port, and topology XML files.
 *
 */
public class LoadIDConfigAction extends MDAction {
	File pluginDir;

	/**
	 * 
	 */
	private static final long serialVersionUID = -6790954285526957354L;
	public LoadIDConfigAction(String id, String name, File pluginDir)
    {
        super(id, name, null, null);
        this.pluginDir = pluginDir;
    }
	
	/**
	 * Runs the LoadIDConfig
	 * 
     * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
     */
    public void actionPerformed(ActionEvent e)
    {
		Project project = Application.getInstance().getProject();
		try {
			LoadIDConfig.process(project, pluginDir);
			Utils.printToAll("*** Completed loading Base IDs/Window IDs from file.");
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			System.out.println("**** " + e1.getMessage());
			System.err.flush();
		}
		//System.err.flush();
  
    }
}
