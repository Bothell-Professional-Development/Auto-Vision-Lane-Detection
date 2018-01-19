package gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.util.HashSet;

import javax.swing.JPanel;

import util.Pair;

public class InteractiveGridPanel extends JPanel
{
    public static final Color DEFAULT_PRIMARY_GRID_COLOR = Color.YELLOW;
    public static final Color DEFAULT_SECONDARY_GRID_COLOR = Color.WHITE;
    public static final Color DEFAULT_POSITIVE_CELL_COLOR = new Color(255, 255, 255, 192);
    public static final Color DEFAULT_NEGATIVE_CELL_COLOR = new Color(255, 0, 0, 192);
    
    private static final long serialVersionUID = 1L;
    
    private boolean m_isRegionOfInterestDivided;
    
    private int m_width;
    private int m_height;
    private int m_horizontalCells;
    private int m_verticalCells;
    
    private BufferedImage m_background;
    private BufferedImage m_frameBuffer;
    private Color m_primaryGridColor;
    private Color m_secondaryGridColor;
    private Color m_positiveCellColor;
    private Color m_negativeCellColor;
    private CellSelector m_cellSelector;
    private RegionOfInterestSelector m_regionOfInterestSelector;
    private Pair<Integer, Integer> m_topLeftRoI;
    private Pair<Integer, Integer> m_bottomRightRoI;
    private Pair<Integer, Integer> m_lastClickedCoord;
    
    public InteractiveGridPanel()
    {
        m_isRegionOfInterestDivided = false;
        m_width = 0;
        m_height = 0;
        m_horizontalCells = 0;
        m_verticalCells = 0;
        m_background = null;
        m_frameBuffer = null;
        m_primaryGridColor = DEFAULT_PRIMARY_GRID_COLOR;
        m_secondaryGridColor = DEFAULT_SECONDARY_GRID_COLOR;
        m_positiveCellColor = DEFAULT_POSITIVE_CELL_COLOR;
        m_negativeCellColor = DEFAULT_NEGATIVE_CELL_COLOR;
        m_cellSelector = new CellSelector(this);
        m_regionOfInterestSelector = new RegionOfInterestSelector(this);
        m_topLeftRoI = null;
        m_bottomRightRoI = null;
        
        this.addMouseListener(m_cellSelector);
        this.addMouseMotionListener(m_regionOfInterestSelector);
    }
    
    public void setBackgroundImageAndRedraw(final BufferedImage img)
    {
        m_background = img;        
        m_width = img.getWidth();
        m_height = img.getHeight();
        
        //copy background to framebuffer, m_background will be used to regenerate areas painted over
        ColorModel color = img.getColorModel();
        WritableRaster raster = img.copyData(null);
        m_frameBuffer =  new BufferedImage(color, raster, color.isAlphaPremultiplied(), null);        
        
        //setSize(m_width, m_height);
        setSize(1536, 864);
        drawGridOnFrameBuffer();
        repaint();
    }
    
    public void setGridDimensions(final int horizontalCells, final int verticalCells)
    {        
        m_horizontalCells = horizontalCells;
        m_verticalCells = verticalCells;
        
        setRegionOfInterest(new Pair<Integer, Integer>(0, 0),
                            new Pair<Integer, Integer>(horizontalCells - 1, verticalCells - 1));

        if(m_background != null)
        {
            setBackgroundImageAndRedraw(m_background);
        }
    }
    
    public void setRegionOfInterest(final Pair<Integer, Integer> topLeft,
                                    final Pair<Integer, Integer> bottomRight)
    {
        if(topLeft != null && bottomRight != null &&
           topLeft.getFirst() >= 0 && topLeft.getFirst() < m_horizontalCells &&
           topLeft.getSecond() >= 0 && topLeft.getSecond() < m_verticalCells)
        {
            if(bottomRight.getFirst() >= 0 && bottomRight.getFirst() < m_horizontalCells &&
               bottomRight.getSecond() >= 0 && bottomRight.getSecond() < m_verticalCells)
            {
                if(bottomRight.getFirst() > topLeft.getFirst() &&
                   bottomRight.getSecond() > topLeft.getSecond())
                {
                    m_topLeftRoI = topLeft;
                    m_bottomRightRoI = bottomRight;
                    
                    if(m_background != null)
                    {
                        setBackgroundImageAndRedraw(m_background);
                    }
                }
            }
        }
    }
    
    protected void setRegionOfInterestUsingMovementListener()
    {
        if(m_regionOfInterestSelector.getDraggedArea() != null)
        {
            Rectangle lastDraggedRect = m_regionOfInterestSelector.getDraggedArea();
            
            Pair<Integer, Integer> topLeft = getCellFromCoordinate((int)lastDraggedRect.getX(),
                                                                   (int)lastDraggedRect.getY());
            Pair<Integer, Integer> botRight = getCellFromCoordinate((int)(lastDraggedRect.getX() + lastDraggedRect.getWidth()),
                                                                    (int)(lastDraggedRect.getY() + lastDraggedRect.getHeight()));
            
            if(topLeft != null && botRight != null)
            {                
                setRegionOfInterest(topLeft, botRight);
            }

            m_regionOfInterestSelector.stopListening();            
        }        
    }
    
    protected void startListeningForRegionOfInterest()
    {
        m_regionOfInterestSelector.startListening();
    }
    
    public void setRegionOfInterestDivided(final boolean divideRegionOfInterest)
    {
        boolean prev = m_isRegionOfInterestDivided;
        
        m_isRegionOfInterestDivided = divideRegionOfInterest;
        
        if(m_isRegionOfInterestDivided != prev)
        {
            drawGridOnFrameBuffer();
            repaint();
        }
    }
    
    public void setPrimaryGridColor(final Color gridColor)
    {
        Color prev = m_primaryGridColor;
        
        m_primaryGridColor = gridColor;

        if(m_primaryGridColor != prev)
        {
            drawGridOnFrameBuffer();
            repaint();
        }
    }
    
    public void setSecondaryGridColor(final Color secondaryGridColor)
    {
        Color prev = m_secondaryGridColor;
        
        m_secondaryGridColor = secondaryGridColor;

        if(m_secondaryGridColor != prev)
        {
            drawGridOnFrameBuffer();
            repaint();
        }
    }
    
    protected void setLastClickedCoord(final Pair<Integer, Integer> coord)
    {
        m_lastClickedCoord = coord;
    }
    
    public boolean isRegionOfInterestDivided()
    {
        return m_isRegionOfInterestDivided;
    }
    
    public int getNumHorizontalCells()
    {
        return m_horizontalCells;
    }
    
    public int getNumVerticalCells()
    {
        return m_verticalCells;
    }
    
    public int getCellWidth()
    {
        return m_width / m_horizontalCells;
    }
    
    public int getCellHeight()
    {
        return m_height / m_verticalCells;
    }
    
    public void setPositiveCellColor(final Color positiveCellColor)
    {
        m_positiveCellColor = positiveCellColor;
        
        eraseAllSelectedCellsOnFrameBuffer();
        drawAllSelectedCellsOnFrameBuffer();
        repaint();
    }
    
    public void setNegativeCellColor(final Color negativeCellColor)
    {
        m_negativeCellColor = negativeCellColor;
        
        eraseAllSelectedCellsOnFrameBuffer();
        drawAllSelectedCellsOnFrameBuffer();
        repaint();
    }

    public Pair<Integer, Integer> getTopLeftRoI()
    {
        return this.m_topLeftRoI;
    }
    
    public Pair<Integer, Integer> getBottomRightRoI()
    {
        return this.m_bottomRightRoI;
    }

    public HashSet<Pair<Integer, Integer>> getPositiveCells()
    {
        return m_cellSelector.getPositiveCells();
    }
    
    public HashSet<Pair<Integer, Integer>> getNegativeCells()
    {
        return m_cellSelector.getNegativeCells();
    }
    
    public void resetSelectedCells()
    {
        m_cellSelector.getPositiveCells().clear();
        m_cellSelector.getNegativeCells().clear();
    }
    
    @Override
    public Dimension getPreferredSize()
    {
        //return new Dimension(m_width, m_height);
        //should not be hardcoded
        return new Dimension(1536, 864);
    }
    
    private Pair<Integer, Integer> getCellFromCoordinate(final int x, final int y)
    {
        Pair<Integer, Integer> clickedCell = null;            

        int scaledCellWidth = (int)Math.ceil((double)getWidth() / getNumHorizontalCells());
        int scaledCellHeight = (int)Math.ceil((double)getHeight() / getNumVerticalCells());
        int regionTopLeftX = m_topLeftRoI.getFirst() * scaledCellWidth;
        int regionTopLeftY = m_topLeftRoI.getSecond() * scaledCellHeight;
        int regionBotRtX = (m_bottomRightRoI.getFirst() + 1) * scaledCellWidth;
        int regionBotRtY = (m_bottomRightRoI.getSecond() + 1) * scaledCellHeight;
        
        if(x > regionTopLeftX && x < regionBotRtX &&
           y > regionTopLeftY && y < regionBotRtY)
        {
            if(getWidth() > 0 && getHeight() > 0)
            {
                int gridXClicked = x / scaledCellWidth;
                int gridYClicked = y / scaledCellHeight;
                
                clickedCell = new Pair<Integer, Integer>(gridXClicked, gridYClicked);
            }
        }
        
        return clickedCell;
    }
    
    private Pair<Integer, Integer> getLastClickedCoord()
    {
        return m_lastClickedCoord;
    }
    
    @Override
    protected void paintComponent(Graphics g)
    {
        super.paintComponent(g);
        
        if(m_frameBuffer != null)
        {
            Rectangle draggedArea = m_regionOfInterestSelector.getDraggedArea();
            
            if(draggedArea != null)
            {
                BufferedImage compositeImage = new BufferedImage(m_frameBuffer.getWidth(),
                                                                 m_frameBuffer.getHeight(),
                                                                 m_frameBuffer.getType());
                
                Graphics2D g2d = compositeImage.createGraphics();

                g2d.drawImage(m_frameBuffer.getScaledInstance(this.getWidth(),
                                                              this.getHeight(),
                                                              Image.SCALE_FAST),
                              0,
                              0,
                              this);
                
                g2d.setColor(m_positiveCellColor);
                
                g2d.fillRect((int)draggedArea.getX(),
                             (int)draggedArea.getY(),
                             (int)draggedArea.getWidth(),
                             (int)draggedArea.getHeight());
                
                g2d.dispose();
                
                g.drawImage(compositeImage,
                            0,
                            0,
                            this);
            }
            else
            {
                g.drawImage(m_frameBuffer.getScaledInstance(this.getWidth(),
                                                            this.getHeight(),
                                                            Image.SCALE_FAST),
                            0,
                            0,
                            this);
            }
        }
    }
    
    private void drawGridOnFrameBuffer()
    {
        if(m_frameBuffer != null)
        {
            int cellWidth = getCellWidth();
            int cellHeight = getCellHeight();
            int regionTopLeftX = m_topLeftRoI.getFirst() * cellWidth;
            int regionTopLeftY = m_topLeftRoI.getSecond() * cellHeight;
            int regionBotRtX = (m_bottomRightRoI.getFirst() + 1) * cellWidth;
            int regionBotRtY = (m_bottomRightRoI.getSecond() + 1) * cellHeight;
            int regionMidPointX = (regionTopLeftX + regionBotRtX) / 2;
            int regionMidPointY = (regionMidPointX / cellWidth) * cellWidth; 

            Graphics2D g2d = m_frameBuffer.createGraphics();
            g2d.setColor(m_primaryGridColor);
            
            for(int x = cellWidth; x < m_width; x += cellWidth)
            {
                if(x >= regionTopLeftX && x <= regionBotRtX)
                {
                    if(m_isRegionOfInterestDivided)
                    {
                        if(x > regionMidPointX)
                        {
                            g2d.setColor(m_secondaryGridColor);
                        }
                    }
                    
                    g2d.drawLine(x, regionTopLeftY, x, regionBotRtY);
                }
            }
            
            for(int y = cellHeight; y < m_height; y += cellHeight)
            {
                g2d.setColor(m_primaryGridColor);
                
                if(y >= regionTopLeftY && y <= regionBotRtY)
                {
                    if(m_isRegionOfInterestDivided)
                    {
                        g2d.drawLine(regionTopLeftX, y, regionMidPointY, y);
                        g2d.setColor(m_secondaryGridColor);
                        g2d.drawLine(regionMidPointY, y, regionBotRtX, y);
                    }
                    else
                    {
                        g2d.drawLine(regionTopLeftX, y, regionBotRtX, y);
                    }
                }
            }
            
            g2d.dispose();
        }
    }

    private void drawSelectedCellOnFrameBuffer(final int column,
    		                                   final int row,
    		                                   final boolean isPositiveCell)
    {
        if(m_frameBuffer != null)
        {
            int cellWidth = getCellWidth();
            int cellHeight = getCellHeight();
    
            Graphics2D g2d = m_frameBuffer.createGraphics();        
            g2d.setColor(isPositiveCell ? m_positiveCellColor : m_negativeCellColor);        
            g2d.fillRect((column * cellWidth) + 1,
                         (row * cellHeight) + 1,
                         cellWidth - 1,
                         cellHeight - 1);
            
            g2d.dispose();
        }
    }        
    
    private void eraseSelectedCellOnFrameBuffer(final int column, final int row)
    {
        if(m_frameBuffer != null)
        {            
            int cellWidth = getCellWidth();
            int cellHeight = getCellHeight();        

            Graphics2D g2d = m_frameBuffer.createGraphics();
            BufferedImage backgroundCell = m_background.getSubimage((column * cellWidth) + 1,
                                                                    (row * cellHeight) + 1,
                                                                    cellWidth - 1,
                                                                    cellHeight - 1);
            g2d.drawImage(backgroundCell,
                          null,
                          (column * cellWidth) + 1,
                          (row * cellHeight) + 1);
            
            g2d.dispose();
        }
    }    

    private void drawAllSelectedCellsOnFrameBuffer()
    {
        for(Pair<Integer, Integer> cell : getPositiveCells())
        {
            drawSelectedCellOnFrameBuffer(cell.getFirst(),
            		                      cell.getSecond(),
            		                      true);
        }
        
        for(Pair<Integer, Integer> cell : getNegativeCells())
        {
            drawSelectedCellOnFrameBuffer(cell.getFirst(),
            		                      cell.getSecond(),
            		                      false);
        }
    }

    private void eraseAllSelectedCellsOnFrameBuffer()
    {
        for(int col = 0; col < m_horizontalCells; ++col)
        {
            for(int row = 0; row < m_verticalCells; ++row)
            {
                eraseSelectedCellOnFrameBuffer(col, row);
            }
        }
    }
    
    private class CellSelector implements MouseListener
    {    	
        private HashSet< Pair<Integer, Integer> > m_positiveCells;
        private HashSet< Pair<Integer, Integer> > m_negativeCells;
        private InteractiveGridPanel m_parent;
        private Pair<Integer, Integer> m_lastClickedCell;
        MouseEvent m_lastPressEvent;
        
        public CellSelector(InteractiveGridPanel parent)
        {
            m_positiveCells = new HashSet< Pair<Integer, Integer> >();
            m_negativeCells = new HashSet< Pair<Integer, Integer> >();
            m_parent = parent;
            m_lastClickedCell = null;
            m_lastPressEvent = null;
        }
        
        public HashSet< Pair<Integer, Integer> > getPositiveCells()
        {
            return m_positiveCells;
        }
        
        public HashSet< Pair<Integer, Integer> > getNegativeCells()
        {
            return m_negativeCells;
        }
        
        @Override
        public void mouseClicked(MouseEvent e){}

        @Override
        public void mouseEntered(MouseEvent e){}

        @Override
        public void mouseExited(MouseEvent e){}

        @Override
        public void mousePressed(MouseEvent e)
        {
        	if(m_lastPressEvent == null)
        	{
                m_lastClickedCell = getClickedCell(e);
                m_lastPressEvent = e;
                m_parent.setLastClickedCoord(new Pair<Integer, Integer>(e.getX(), e.getY()));
        	}
        }

        @Override
        public void mouseReleased(MouseEvent e)
        {
        	if(m_lastPressEvent != null)
        	{
    	        if(e.getButton() == MouseEvent.BUTTON1 ? m_lastPressEvent.getButton() == MouseEvent.BUTTON1 :
                    m_lastPressEvent.getButton() == MouseEvent.BUTTON3)
				{
					Pair<Integer, Integer> clickedCell = getClickedCell(e);
					
					if(m_lastClickedCell != null && clickedCell != null &&
					   m_lastClickedCell.getFirst() == clickedCell.getFirst() &&
					   m_lastClickedCell.getSecond() == clickedCell.getSecond())
					{
						selectCell(m_lastClickedCell,
								   e.getButton() == MouseEvent.BUTTON1 ? true : false);
					}
					
					m_lastClickedCell = null;
				}
    	        
				m_lastPressEvent = null;
        	}
            
            m_parent.setRegionOfInterestUsingMovementListener();
        }
        
        private Pair<Integer, Integer> getClickedCell(final MouseEvent e)
        {
            return m_parent.getCellFromCoordinate(e.getX(), e.getY());
        }
        
        private void selectCell(final Pair<Integer, Integer> clickedCell,
        		                final boolean isPositiveSelection)
        {
            if(clickedCell != null)
            {
                if(!m_positiveCells.contains(clickedCell) &&
                   !m_negativeCells.contains(clickedCell))
                {
                	if(isPositiveSelection)
                	{
                        m_positiveCells.add(clickedCell);

                	}
                	else
                	{
                        m_negativeCells.add(clickedCell);
                	}
                	
                    m_parent.drawSelectedCellOnFrameBuffer(clickedCell.getFirst(),
                                                           clickedCell.getSecond(),
                                                           isPositiveSelection);
                }
                else
                {
                	if(m_positiveCells.contains(clickedCell))
                	{
                		m_positiveCells.remove(clickedCell);
                	}
                	
                	if(m_negativeCells.contains(clickedCell))
                	{
                		m_negativeCells.remove(clickedCell);
                	}
                	
                    m_parent.eraseSelectedCellOnFrameBuffer(clickedCell.getFirst(),
                                                            clickedCell.getSecond());
                }
                
                m_parent.repaint();
            }        
        }
    }
    
    private class RegionOfInterestSelector implements MouseMotionListener
    {
        private InteractiveGridPanel m_parent;
        private Rectangle m_draggedArea;
        
        RegionOfInterestSelector(final InteractiveGridPanel parent)
        {
            m_parent = parent;
            m_draggedArea = null;
        }

        @Override
        public void mouseDragged(MouseEvent e)
        {
            if(m_draggedArea != null)
            {
                Pair<Integer, Integer> lastClickedCoord = m_parent.getLastClickedCoord();
                
                Pair<Integer, Integer> topLeft = new Pair<Integer, Integer>(Math.min(lastClickedCoord.getFirst(), e.getX()),
                                                                            Math.min(lastClickedCoord.getSecond(), e.getY()));
                Pair<Integer, Integer> botRight = new Pair<Integer, Integer>(Math.max(lastClickedCoord.getFirst(), e.getX()),
                                                                             Math.max(lastClickedCoord.getSecond(), e.getY()));
                
                m_draggedArea.setBounds(topLeft.getFirst(),
                                        topLeft.getSecond(),
                                        botRight.getFirst() - topLeft.getFirst(),
                                        botRight.getSecond() - topLeft.getSecond());
                m_parent.repaint();
            }
        }

        @Override
        public void mouseMoved(MouseEvent e){}
        
        private Rectangle getDraggedArea()
        {
            return m_draggedArea;
        }
        
        private void startListening()
        {
            m_draggedArea = new Rectangle();
        }
        
        private void stopListening()
        {
            m_draggedArea = null;
        }
    }
}