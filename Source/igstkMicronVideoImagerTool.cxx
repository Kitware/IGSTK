
#include "igstkMicronVideoImagerTool.h"
#include "igstkMicronVideoImager.h"

namespace igstk
{

  /** Constructor */
  MicronVideoImagerTool::MicronVideoImagerTool():m_StateMachine(this)
  {
    m_VideoImagerToolConfigured = false;

    // States
    igstkAddStateMacro( Idle );
    igstkAddStateMacro( VideoImagerToolNameSpecified );

    // Set the input descriptors
    igstkAddInputMacro( ValidVideoImagerToolName );
    igstkAddInputMacro( InValidVideoImagerToolName );

    // Add transitions
    // Transitions from idle state
    igstkAddTransitionMacro( Idle,
        ValidVideoImagerToolName,
        VideoImagerToolNameSpecified,
        SetVideoImagerToolName );

    igstkAddTransitionMacro( Idle,
        InValidVideoImagerToolName,
        Idle,
        ReportInvalidVideoImagerToolNameSpecified );

    // Transitions from VideoImagerToolName specified
    igstkAddTransitionMacro( VideoImagerToolNameSpecified,
        ValidVideoImagerToolName,
        VideoImagerToolNameSpecified,
        ReportInvalidRequest );
    igstkAddTransitionMacro( VideoImagerToolNameSpecified,
        InValidVideoImagerToolName,
        VideoImagerToolNameSpecified,
        ReportInvalidRequest );

    igstkSetInitialStateMacro( Idle );

    m_StateMachine.SetReadyToRun();
  }

  /** Destructor */
  MicronVideoImagerTool::~MicronVideoImagerTool()
  {
  }

  /** Request set VideoImagerTool name */
  void MicronVideoImagerTool::RequestSetVideoImagerToolName( const std::string& clientDeviceName )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImagerTool::RequestSetVideoImagerToolName called ...\n");
    if ( clientDeviceName == "" )
    {
      m_StateMachine.PushInput( m_InValidVideoImagerToolNameInput );
      m_StateMachine.ProcessInputs();
    }
    else
    {
      m_VideoImagerToolNameToBeSet = clientDeviceName;
      m_StateMachine.PushInput( m_ValidVideoImagerToolNameInput );
      m_StateMachine.ProcessInputs();
    }
  }

  /** Set valid VideoImagerTool name */
  void MicronVideoImagerTool::SetVideoImagerToolNameProcessing( )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImagerTool::SetVideoImagerToolNameProcessing called ...\n");

    this->m_VideoImagerToolName = m_VideoImagerToolNameToBeSet;

    m_VideoImagerToolConfigured = true;

    // VideoImagerTool name is used as a unique identifier
    this->SetVideoImagerToolIdentifier( this->m_VideoImagerToolName );

  }

  /** Report Invalid VideoImagerTool name*/
  void MicronVideoImagerTool::ReportInvalidVideoImagerToolNameSpecifiedProcessing( )
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImagerTool::ReportInvalidVideoImagerToolNameSpecifiedProcessing "
        "called ...\n");

    igstkLogMacro( CRITICAL, "Invalid VideoImagerTool name specified ");
  }

  void MicronVideoImagerTool::ReportInvalidRequestProcessing()
  {
    igstkLogMacro( WARNING, "ReportInvalidRequestProcessing() called ...\n");
  }

  /** The "CheckIfVideoImagerToolIsConfigured" method returns true if the imager
  * tool is configured */
  bool
  MicronVideoImagerTool::CheckIfVideoImagerToolIsConfigured( ) const
  {
    igstkLogMacro( DEBUG,
        "igstk::MicronVideoImagerTool::CheckIfVideoImagerToolIsConfigured called...\n");
    return m_VideoImagerToolConfigured;
  }

  /** Print Self function */
  void MicronVideoImagerTool::PrintSelf( std::ostream& os, itk::Indent indent ) const
  {
    Superclass::PrintSelf(os, indent);

    os << indent << "VideoImagerTool name : "    << m_VideoImagerToolName << std::endl;
    os << indent << "VideoImagerToolConfigured:" << m_VideoImagerToolConfigured << std::endl;
  }

}  // namespace igstk

