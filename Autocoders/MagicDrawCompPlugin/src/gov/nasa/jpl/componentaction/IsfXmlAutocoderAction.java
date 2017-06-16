/**
 * $Id: ActionTypesExample.java 52410 2007-10-05 06:51:43Z donsim $
 *
 * Copyright (c) 2002 NoMagic, Inc. All Rights Reserved.
 */
package gov.nasa.jpl.componentaction;
 
import com.nomagic.actions.ActionsCategory;
import com.nomagic.actions.NMAction;
import com.nomagic.magicdraw.actions.ActionsConfiguratorsManager;
import com.nomagic.magicdraw.plugins.Plugin;

/**
 * Used to create generation/about tabs within MagicDraw. Buttons are attached to different action object.
 *
 */
public class IsfXmlAutocoderAction extends Plugin
{

	/**
	 * @see com.nomagic.magicdraw.plugins.Plugin#init()
	 */
	public void init()
	{
		//System.out.println("My Plugin\n");
	  //javax.swing.JOptionPane.showMessageDialog(null, "My Plugin init");
		
		ActionsConfiguratorsManager manager = ActionsConfiguratorsManager.getInstance();
		manager.addMainMenuConfigurator(new MainMenuConfigurator(getSeparatedActions()));
		
	}

	/**
	 * @see com.nomagic.magicdraw.plugins.Plugin#close()
	 */
	public boolean close()
	{
		return true;
	}

	/**
	 * @see com.nomagic.magicdraw.plugins.Plugin#isSupported()
	 */
	public boolean isSupported()
	{
		return true;
	}

	/**
	 * Creates group of actions. This group is separated from others using menu separator (when it represented in menu).
	 * Separator is added for group of actions in one actions category.
	 */
	private NMAction getSeparatedActions()
	{
		ActionsCategory category = new ActionsCategory(null, null);
		category.addAction(new IsfXmlAction(null,"Auto Generate Component/Port/Topology XML", this.getDescriptor().getPluginDirectory()));
		category.addAction(new IsfComponentAction(null,"Component and Port Only XML", this.getDescriptor().getPluginDirectory()));
		category.addAction(new IsfTopAction(null,"Topology Only XML", this.getDescriptor().getPluginDirectory()));
		category.addAction(new LoadIDConfigAction(null,"Load Base ID/Window ID Config CSV", this.getDescriptor().getPluginDirectory()));
		category.addAction(new IsfAbout(null,"About", this.getDescriptor().getPluginDirectory()));
		return category;
	}

}

