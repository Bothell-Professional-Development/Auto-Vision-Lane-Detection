package gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;

import javax.swing.AbstractButton;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.WindowConstants;
import javax.swing.filechooser.FileNameExtensionFilter;

import core.SVMTrainingDataToolApplication;
import util.Pair;

public class SVMTrainingDataGUI
{    
    private JFrame m_frame;
    private InteractiveGridPanel m_gridPanel;
    private SVMTrainingDataToolApplication m_parentApplication;
    
    public SVMTrainingDataGUI(final SVMTrainingDataToolApplication parentApplication)
    {
        m_frame = new JFrame();
        m_gridPanel = new InteractiveGridPanel();
        m_parentApplication = parentApplication;

        configureLayout();        

        m_frame.setTitle("SVM Training Data Generation Tool");
        m_frame.setVisible(true);
        m_frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        m_frame.setResizable(false);
        m_frame.pack();
    }
    
    public void setBackgroundImageAndRedraw(final BufferedImage img)
    {
        m_gridPanel.setBackgroundImageAndRedraw(img);
        m_frame.pack();
    }
    
    public void setGridDimensions(final int horizontalCells,
                                  final int verticalCells)
    {
        m_gridPanel.setGridDimensions(horizontalCells,
                                      verticalCells);
    }
    
    public void setRegionOfInterest(final Pair<Integer, Integer> topLeft,
                                    final Pair<Integer, Integer> bottomRight)
    {
        m_gridPanel.setRegionOfInterest(topLeft,
                                        bottomRight);
    }
    
    public void setRegionOfInterestDivided(final boolean divideRegionOfInterest)
    {
        m_gridPanel.setRegionOfInterestDivided(divideRegionOfInterest);
    }
    
    public void setPostiveCellColor(final Color positiveCellColor)
    {
        m_gridPanel.setPositiveCellColor(positiveCellColor);
    }
    
    public void setNegativeCellColor(final Color negativeCellColor)
    {
        m_gridPanel.setPositiveCellColor(negativeCellColor);
    }
    
    public InteractiveGridPanel getGridPanel()
    {
        return m_gridPanel;
    }

    public void showEoFDialog()
    {
        JOptionPane.showMessageDialog(m_frame,
                                     "End of video file has been reached!",
                                     "Finished!",
                                     JOptionPane.INFORMATION_MESSAGE);
    }

    public void showWriteErrorDialog()
    {
        JOptionPane.showMessageDialog(m_frame,
                                      "Could not write files to disk!",
                                      "I/O Error",
                                      JOptionPane.ERROR_MESSAGE);
        System.exit(0);
    }

    public void showInvalidOutputPathErrorDialog()
    {
        JOptionPane.showMessageDialog(m_frame,
                                      "Supplied output path(s) either invalid or nonexistent",
                                      "Invalid Output Path!",
                                      JOptionPane.ERROR_MESSAGE);
        System.exit(0);
    }
    
    private void configureLayout()
    {
        Dimension preferredSize = new Dimension(150, 30);
        
        JMenuBar menuBar = createMenuBar();
        JButton generateButton = createGenerateButton(preferredSize);
        JButton nextButton = createNextButton(preferredSize);
        
        m_frame.setLayout(new BorderLayout());
        m_frame.add(menuBar, BorderLayout.NORTH);        
        m_frame.add(m_gridPanel);
        
        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new FlowLayout());
        buttonPanel.add(generateButton);
        buttonPanel.add(nextButton);
        m_frame.add(buttonPanel, BorderLayout.SOUTH);
    }
    
    private JMenuBar createMenuBar()
    {
        JMenuBar menuBar = new JMenuBar();
        
        JMenu fileMenu = new JMenu("File");
        JMenuItem openItem = createFileMenuItem();
                
        fileMenu.add(openItem);
    
        JMenu settingsMenu = new JMenu("Settings");
        JMenu gridSubMenu = createGridDimensionSubMenu();
        JMenu divideGridSubMenu = createDivideGridSubMenu();
        JMenu gridColorSubMenu = createGridColorSubMenu();
        JMenuItem frameSkipItem = createFrameSkipMenuItem();
        JMenuItem setRegionOfInterestItem = createSetRegionOfInterestItem();

        settingsMenu.add(gridSubMenu);
        settingsMenu.add(divideGridSubMenu);
        settingsMenu.add(gridColorSubMenu);
        settingsMenu.add(frameSkipItem);
        settingsMenu.add(setRegionOfInterestItem);
        
        menuBar.add(fileMenu);
        menuBar.add(settingsMenu);
        
        return menuBar;
    }
    
    private JButton createGenerateButton(final Dimension preferredSize)
    {
        JButton generateButton = new JButton();
        generateButton.setPreferredSize(preferredSize);
        generateButton.setText("Generate Images");
        
        generateButton.addActionListener(new ActionListener()
                                                {
                                                    public void actionPerformed(ActionEvent e)
                                                    {
                                                        if(!m_parentApplication.writeImagesToDisk(m_gridPanel.getTopLeftRoI(),
                                                                                                  m_gridPanel.getBottomRightRoI(),
                                                                                                  m_gridPanel.isRegionOfInterestDivided()))
                                                        {
                                                            showWriteErrorDialog();
                                                        }
                                                        else
                                                        {
                                                            m_parentApplication.displayNextFrame();                                             
                                                        }
                                                    }
                                                }
                                         );
        return generateButton;
    }
    
    private JButton createNextButton(final Dimension preferredSize)
    {
        JButton nextButton = new JButton();
        nextButton.setPreferredSize(preferredSize);
        nextButton.setText("Next Frame");
        
        nextButton.addActionListener(new ActionListener()
                                           {
                                               public void actionPerformed(ActionEvent e)
                                               {
                                                   boolean displayNext = true;
                                                   
                                                   if(m_gridPanel.getPositiveCells().size() > 0 || m_gridPanel.getNegativeCells().size() > 0)
                                                   {
                                                       String message = "There are selected grid cells. If you continue, no training files will be written!";
                                                       int decision = JOptionPane.showConfirmDialog(m_frame, 
                                                                                                    message,
                                                                                                    "Warning",
                                                                                                    JOptionPane.YES_NO_OPTION);
                                                       if(decision == JOptionPane.NO_OPTION)
                                                       {
                                                           displayNext = false;
                                                       }
                                                   }
                                                   
                                                   if(displayNext)
                                                   {
                                                       m_parentApplication.displayNextFrame();
                                                   }                                                  
                                               } 
                                           } 
                                     );        
        return nextButton;
    }
    
    private JMenuItem createFileMenuItem()
    {
        JMenuItem openItem = new JMenuItem("Open");
        openItem.addActionListener(new ActionListener()
                                       {
                                           @Override
                                           public void actionPerformed(ActionEvent arg0)
                                           {
                                               JFileChooser fc = new JFileChooser("Select Input File");
                                               fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
                                               FileNameExtensionFilter videoFormats = new FileNameExtensionFilter("Video Formats",
                                                                                                                  "avi",
                                                                                                                  "mov",
                                                                                                                  "mp4",
                                                                                                                  "mpeg",
                                                                                                                  "mpg",
                                                                                                                  "wmv");
                                               fc.addChoosableFileFilter(videoFormats);
                                               fc.setFileFilter(videoFormats);
                                               
                                               if(fc.showOpenDialog(m_frame) == JFileChooser.APPROVE_OPTION)
                                               {
                                                   m_parentApplication.setSourceFile(fc.getSelectedFile().getPath());
                                               }
                                           }                                       
                                       }
                                  );
        return openItem;
    }
    
    private JMenuItem createPrimaryGridColorMenuItem()
    {
        JMenuItem gridColorItem = new JMenuItem("Primary/Left Grid Color");
        gridColorItem.addActionListener(new ActionListener()
                                            {
                                                @Override
                                                public void actionPerformed(ActionEvent arg0)
                                                {
                                                    Color color = JColorChooser.showDialog(m_frame,
                                                                                           "Primary/Left Grid Color",
                                                                                           InteractiveGridPanel.DEFAULT_PRIMARY_GRID_COLOR);
                                                    m_gridPanel.setPrimaryGridColor(color);
                                                }
                                            }
                                       );
        return gridColorItem;
    }    
    
    private JMenuItem createSecondaryGridColorMenuItem()
    {
        JMenuItem gridColorItem = new JMenuItem("Right Grid Color");
        gridColorItem.addActionListener(new ActionListener()
                                            {
                                                @Override
                                                public void actionPerformed(ActionEvent arg0)
                                                {
                                                    Color color = JColorChooser.showDialog(m_frame,
                                                                                           "Left Grid Color",
                                                                                           InteractiveGridPanel.DEFAULT_SECONDARY_GRID_COLOR);
                                                    m_gridPanel.setSecondaryGridColor(color);
                                                }
                                            }
                                       );
        return gridColorItem;
    }
    
    private JMenuItem createPositiveSelectionColorMenuItem()
    {
        JMenuItem gridHighlightColorItem = new JMenuItem("Positive Selection Color");
        gridHighlightColorItem.addActionListener(new ActionListener()
                                                     {
                                                         @Override
                                                         public void actionPerformed(ActionEvent arg0)
                                                         {
                                                             Color color = JColorChooser.showDialog(m_frame,
                                                                                                    "Positive Selection Color",
                                                                                                    InteractiveGridPanel.DEFAULT_POSITIVE_CELL_COLOR);
                                                             m_gridPanel.setPositiveCellColor(color);
                                                         }
                                                     }
                                                );
        return gridHighlightColorItem;
    }    
    
    private JMenuItem createNegativeSelectionColorMenuItem()
    {
        JMenuItem gridHighlightColorItem = new JMenuItem("Negative Selection Color");
        gridHighlightColorItem.addActionListener(new ActionListener()
                                                     {
                                                         @Override
                                                         public void actionPerformed(ActionEvent arg0)
                                                         {
                                                             Color color = JColorChooser.showDialog(m_frame,
                                                                                                    "Negative Selection Color",
                                                                                                    InteractiveGridPanel.DEFAULT_NEGATIVE_CELL_COLOR);
                                                             m_gridPanel.setNegativeCellColor(color);
                                                         }
                                                     }
                                                );
        return gridHighlightColorItem;
    }
    
    private JMenuItem createFrameSkipMenuItem()
    {
        JMenuItem frameSkipItem = new JMenuItem("Frame Skip");
        frameSkipItem.addActionListener(new ActionListener()
                                            {
                                                  @Override
                                                public void actionPerformed(ActionEvent arg0)
                                                {
                                                      String stringInput = null;
                                                      int framesToSkip = Integer.MIN_VALUE;
                                                      
                                                      while(framesToSkip < 0)
                                                      {
                                                          stringInput = JOptionPane.showInputDialog("Enter number of frames to skip.");
                                                          try{
                                                              framesToSkip = Integer.parseInt(stringInput);
                                                              
                                                              if(framesToSkip < 0)
                                                              {
                                                                  JOptionPane.showMessageDialog(m_frame, "Value entered must be positive.");
                                                              }
                                                          }
                                                          catch(NumberFormatException e)
                                                          {
                                                              JOptionPane.showMessageDialog(m_frame, "Value entered is not a number.");
                                                          }
                                                      }
                                                      
                                                      m_parentApplication.setFrameSkip(framesToSkip);
                                                }
                                            }
                                       );
        return frameSkipItem;
    }
    
    private JMenuItem createSetRegionOfInterestItem()
    {
        JMenuItem setRegionOfInterestItem = new JMenuItem("Set RoI");
        setRegionOfInterestItem.addActionListener(new ActionListener()
                                                      {
                                                          @Override
                                                          public void actionPerformed(ActionEvent arg0)
                                                          {
                                                              m_gridPanel.setGridDimensions(m_gridPanel.getNumHorizontalCells(),
                                                                                            m_gridPanel.getNumVerticalCells());
                                                              
                                                              JOptionPane.showMessageDialog(m_frame,
                                                                      "Drag area in window to select desired region of interest.",
                                                                      "Select RoI",
                                                                      JOptionPane.INFORMATION_MESSAGE);
                                                              
                                                              m_gridPanel.startListeningForRegionOfInterest();
                                                          }
                                                      }
                                                 );
        return setRegionOfInterestItem;
    }
    
    private JMenu createGridDimensionSubMenu()
    {
        JMenu gridDimensionMenu = new JMenu("Grid Dimensions");
        ButtonGroup dimensionButtons = new ButtonGroup();
        
        JRadioButtonMenuItem twentyByTwenty = new JRadioButtonMenuItem("96 x 54");
        JRadioButtonMenuItem thirtyByThirty = new JRadioButtonMenuItem("64 x 36", true);
        JRadioButtonMenuItem fortyByForty = new JRadioButtonMenuItem("48 x 27");
        JRadioButtonMenuItem sixtyBySixty = new JRadioButtonMenuItem("32 x 18");
        JRadioButtonMenuItem oneTwentyByOneTwenty = new JRadioButtonMenuItem("16 x 9");
        
        ActionListener radioListener = new ActionListener()
                                           {
                                               @Override
                                               public void actionPerformed(ActionEvent arg0)
                                               {
                                                   AbstractButton radioOption = (AbstractButton) arg0.getSource();
                                                   String label = radioOption.getText();
                                                   String[] dimensions = label.split(" x ");
                                                   m_gridPanel.setGridDimensions(Integer.parseInt(dimensions[0]),
                                                                                 Integer.parseInt(dimensions[1]));                                                   
                                               }                                                    
                                           };
                                           
        twentyByTwenty.addActionListener(radioListener);
        thirtyByThirty.addActionListener(radioListener);
        fortyByForty.addActionListener(radioListener);
        sixtyBySixty.addActionListener(radioListener);
        oneTwentyByOneTwenty.addActionListener(radioListener);
        
        dimensionButtons.add(twentyByTwenty);
        dimensionButtons.add(thirtyByThirty);
        dimensionButtons.add(fortyByForty);
        dimensionButtons.add(sixtyBySixty);
        dimensionButtons.add(oneTwentyByOneTwenty);
        
        gridDimensionMenu.add(twentyByTwenty);
        gridDimensionMenu.add(thirtyByThirty);
        gridDimensionMenu.add(fortyByForty);
        gridDimensionMenu.add(sixtyBySixty);
        gridDimensionMenu.add(oneTwentyByOneTwenty);
        
        return gridDimensionMenu;
    }
    
    private JMenu createDivideGridSubMenu()
    {
        JMenu divideGridMenu = new JMenu("Divide Grid");
        ButtonGroup buttons = new ButtonGroup();
        
        JRadioButtonMenuItem yesButton = new JRadioButtonMenuItem("Yes", true);
        JRadioButtonMenuItem noButton = new JRadioButtonMenuItem("No", true);
        
        ActionListener radioListener = new ActionListener()
                                           {
                                               @Override
                                               public void actionPerformed(ActionEvent arg0)
                                               {
                                                   AbstractButton radioOption = (AbstractButton) arg0.getSource();
                                                   String label = radioOption.getText();
                                                   m_gridPanel.setRegionOfInterestDivided(label.equals("Yes") ? true : false);                                               
                                               }                                                    
                                           };
                                           
        yesButton.addActionListener(radioListener);
        noButton.addActionListener(radioListener);
        
        buttons.add(yesButton);
        buttons.add(noButton);
        
        divideGridMenu.add(yesButton);
        divideGridMenu.add(noButton);
        
        return divideGridMenu;
    }
    
    private JMenu createGridColorSubMenu()
    {
        JMenu colorMenu = new JMenu("Grid Colors");

        colorMenu.add(createPrimaryGridColorMenuItem());
        colorMenu.add(createSecondaryGridColorMenuItem());
        colorMenu.add(createPositiveSelectionColorMenuItem());
        colorMenu.add(createNegativeSelectionColorMenuItem());
        
        return colorMenu;
    }
}