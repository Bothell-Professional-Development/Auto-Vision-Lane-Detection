package core;

import java.awt.image.BufferedImage;

import gui.SVMTrainingDataGUI;
import util.Pair;

public class SVMTrainingDataToolApplication
{	
	private static final int DEFAULT_FRAMES_TO_SKIP = 150;
	private static final int DEFAULT_HORIZONTAL_GRID_CELLS = 64;
	private static final int DEFAULT_VERTICAL_GRID_CELLS = 36;
	private static final Pair<Integer, Integer> DEFAULT_ROI_TOP_LEFT = new Pair<Integer, Integer>(17, 17);
	private static final Pair<Integer, Integer> DEFAULT_ROI_BOT_RIGHT = new Pair<Integer, Integer>(48, 25);
	
	private SVMTrainingDataHandler m_handler;
	private SVMTrainingDataGUI m_gui;
	
	public SVMTrainingDataToolApplication()
	{
		m_handler = new SVMTrainingDataHandler();
		m_gui = new SVMTrainingDataGUI(this);
		
		setFrameSkip(DEFAULT_FRAMES_TO_SKIP);
		setGridDimensions(DEFAULT_HORIZONTAL_GRID_CELLS,
				          DEFAULT_VERTICAL_GRID_CELLS);
		m_gui.setRegionOfInterest(DEFAULT_ROI_TOP_LEFT, 
				                  DEFAULT_ROI_BOT_RIGHT);
	}
	
	public boolean setSourceFile(final String filename)
	{
		boolean retVal = m_handler.setSourceFile(filename);		
		displayNextFrame();
		
		return retVal;
	}
	
	public void setFrameSkip(final int framesToSkip)
	{
		m_handler.setFrameSkip(framesToSkip);
	}
	
	public void setGridDimensions(final int horizontalCells, final int verticalCells)
	{
		m_gui.setGridDimensions(horizontalCells,
                                verticalCells);
	}
	
	public void setPositiveOutputPath(final String path)
	{
		if(!m_handler.setPositiveOutputPath(path))
		{
			m_gui.showInvalidOutputPathErrorDialog();
		}
	}
	
	public void setNegativeOutputPath(final String path)
	{
		if(!m_handler.setNegativeOutputPath(path))
		{
			m_gui.showInvalidOutputPathErrorDialog();
		}
	}
	
	public void displayNextFrame()
	{
		BufferedImage nextFrame = m_handler.getNextFrame();
		
		if(nextFrame != null)
		{
			m_gui.getGridPanel().resetSelectedCells();
			m_gui.setBackgroundImageAndRedraw(nextFrame);
		}
		else
		{
			m_gui.showEoFDialog();
		}
	}

	public boolean writeImagesToDisk(final Pair<Integer, Integer> topLeftRoI,
                                     final Pair<Integer, Integer> botRightRoI)
	{
		return m_handler.writeImagesToDisk(m_gui.getGridPanel().getSelectedCells(),
							               m_gui.getGridPanel().getNumHorizontalCells(),
							               m_gui.getGridPanel().getNumVerticalCells(),
							               topLeftRoI,
							               botRightRoI);
	}	
}
 