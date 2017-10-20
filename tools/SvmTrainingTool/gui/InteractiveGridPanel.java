package gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.WritableRaster;
import java.util.HashSet;

import javax.swing.JPanel;

import util.Pair;

public class InteractiveGridPanel extends JPanel
{
    public static final Color DEFAULT_GRID_COLOR = Color.YELLOW;
    public static final Color DEFAULT_SELECTED_CELL_COLOR = new Color(255, 255, 255, 192);
    
    private static final long serialVersionUID = 1L;
    
    private int m_width;
    private int m_height;
    private int m_horizontalCells;
    private int m_verticalCells;
    
    private BufferedImage m_background;
    private BufferedImage m_frameBuffer;
    private Color m_gridColor;
    private Color m_selectedCellColor;
    private CellSelector m_cellSelector;
    private Pair<Integer, Integer> m_topLeftRoI;
    private Pair<Integer, Integer> m_bottomRightRoI;
    
    public InteractiveGridPanel()
    {
        m_width = 0;
        m_height = 0;
        m_horizontalCells = 0;
        m_verticalCells = 0;
        m_background = null;
        m_frameBuffer = null;
        m_gridColor = DEFAULT_GRID_COLOR;
        m_selectedCellColor = DEFAULT_SELECTED_CELL_COLOR;
        m_cellSelector = new CellSelector(this);
        m_topLeftRoI = null;
        m_bottomRightRoI = null;
        
        this.addMouseListener(m_cellSelector);
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
            //drawGridOnFrameBuffer();        
            //repaint();
        }
    }
    
    public void setRegionOfInterest(final Pair<Integer, Integer> topLeft,
                                    final Pair<Integer, Integer> bottomRight)
    {
        if(topLeft.getFirst() >= 0 && topLeft.getFirst() < m_horizontalCells &&
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
    
    public void setGridColor(final Color gridColor)
    {
        m_gridColor = gridColor;

        drawGridOnFrameBuffer();
        repaint();
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
    
    public void setSelectedCellColor(final Color selectedCellColor)
    {
        m_selectedCellColor = selectedCellColor;
        
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

    public HashSet<Pair<Integer, Integer>> getSelectedCells()
    {
        return m_cellSelector.getSelectedCells();
    }
    
    public void resetSelectedCells()
    {
        m_cellSelector.getSelectedCells().clear();
    }
    
    @Override
    public Dimension getPreferredSize()
    {
        //return new Dimension(m_width, m_height);
        //should not be hardcoded
        return new Dimension(1536, 864);
    }

    @Override
    protected void paintComponent(Graphics g)
    {
        super.paintComponent(g);
        
        if(m_frameBuffer != null)
        {            
            g.drawImage(m_frameBuffer.getScaledInstance(this.getWidth(),
                                                        this.getHeight(),
                                                        Image.SCALE_FAST),
                        0,
                        0,
                        this);
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

            Graphics2D g2d = m_frameBuffer.createGraphics();
            g2d.setColor(m_gridColor);
            
            for(int x = cellWidth; x < m_width; x += cellWidth)
            {
                if(x >= regionTopLeftX && x <= regionBotRtX)
                {
                    //g2d.drawLine(x, 0, x, m_height);
                    g2d.drawLine(x, regionTopLeftY, x, regionBotRtY);
                }
            }
            
            for(int y = cellHeight; y < m_height; y += cellHeight)
            {
                if(y >= regionTopLeftY && y <= regionBotRtY)
                {
                    //g2d.drawLine(0, y, m_width, y);
                    g2d.drawLine(regionTopLeftX, y, regionBotRtX, y);
                }
            }
        }
    }

    private void drawSelectedCellOnFrameBuffer(final int column, final int row)
    {
        if(m_frameBuffer != null)
        {
            int cellWidth = getCellWidth();
            int cellHeight = getCellHeight();
    
            Graphics2D g2d = m_frameBuffer.createGraphics();        
            g2d.setColor(m_selectedCellColor);        
            g2d.fillRect((column * cellWidth) + 1,
                            (row * cellHeight) + 1,
                          cellWidth - 1,
                          cellHeight - 1);
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
        }
    }    

    private void drawAllSelectedCellsOnFrameBuffer()
    {
        for(Pair<Integer, Integer> cell : getSelectedCells())
        {
            drawSelectedCellOnFrameBuffer(cell.getFirst(), cell.getSecond());
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
    
    class CellSelector implements MouseListener
    {
        private HashSet< Pair<Integer, Integer> > m_selectedCells;
        private InteractiveGridPanel m_parent;
        private Pair<Integer, Integer> m_lastClickedCell;
        
        public CellSelector(InteractiveGridPanel parent)
        {
            m_selectedCells = new HashSet< Pair<Integer, Integer> >();
            m_parent = parent;
            m_lastClickedCell = null;
        }
        
        public HashSet< Pair<Integer, Integer> > getSelectedCells()
        {
            return m_selectedCells;
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
            m_lastClickedCell = getClickedCell(e); 
        }

        @Override
        public void mouseReleased(MouseEvent e)
        {
            Pair<Integer, Integer> clickedCell = getClickedCell(e);
            
            if(m_lastClickedCell != null &&
               m_lastClickedCell.getFirst() == clickedCell.getFirst() &&
               m_lastClickedCell.getSecond() == clickedCell.getSecond())
            {
                toggleCell(m_lastClickedCell);
            }
            
            m_lastClickedCell = null;
        }
        
        private Pair<Integer, Integer> getClickedCell(final MouseEvent e)
        {
            Pair<Integer, Integer> clickedCell = null;            

            int scaledCellWidth = (int)Math.ceil((double)m_parent.getWidth() / m_parent.getNumHorizontalCells());
            int scaledCellHeight = (int)Math.ceil((double)m_parent.getHeight() / m_parent.getNumVerticalCells());
            int regionTopLeftX = m_topLeftRoI.getFirst() * scaledCellWidth;
            int regionTopLeftY = m_topLeftRoI.getSecond() * scaledCellHeight;
            int regionBotRtX = (m_bottomRightRoI.getFirst() + 1) * scaledCellWidth;
            int regionBotRtY = (m_bottomRightRoI.getSecond() + 1) * scaledCellHeight;
            
            if(e.getX() >= regionTopLeftX && e.getX() <= regionBotRtX &&
               e.getY() >= regionTopLeftY && e.getY() <= regionBotRtY)
            {
                if(m_parent.getWidth() > 0 && m_parent.getHeight() > 0)
                {
                    int gridXClicked = e.getX() / scaledCellWidth;
                    int gridYClicked = e.getY() / scaledCellHeight;
                    
                    clickedCell = new Pair<Integer, Integer>(gridXClicked, gridYClicked);
                }
            }
            
            return clickedCell;
        }
        
        private void toggleCell(final Pair<Integer, Integer> clickedCell)
        {
            if(clickedCell != null)
            {                
                if(!m_selectedCells.contains(clickedCell))
                {                    
                    m_selectedCells.add(clickedCell);
                    m_parent.drawSelectedCellOnFrameBuffer(clickedCell.getFirst(),
                                                           clickedCell.getSecond());
                    m_parent.repaint();
                }
                else
                {
                    m_selectedCells.remove(clickedCell);
                    m_parent.eraseSelectedCellOnFrameBuffer(clickedCell.getFirst(),
                                                            clickedCell.getSecond());
                    m_parent.repaint();
                }
            }        
        }
    }
}