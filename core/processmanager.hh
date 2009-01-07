#ifndef PROCESS_MANAGER_HH
#define PROCESS_MANAGER_HH

#include <QObject>
#include <QMap>
#include <QProcess>




namespace qtablet
{
    //! ProcessData stores is a class for additional data about the process.
    //!
    //! Class Process stores process' name or path and the pid.
    //! This data will be used e.g. on crash situations for informing the user
    //! about the application crash.
    //!
    class ProcessData
    {
      public:
        
        //! Default CTor.
        ProcessData():
            m_name(),
            m_pid()
        {
        }
      
        //! Ctor.
        //! \param pid process id
        //! \name process name or path
        ProcessData( Q_PID const & pid, QString const & name ):
            m_name( name ),
            m_pid ( pid )
        {              
        }
        
        //! Dtor. Does nothing.        
        virtual ~ProcessData()
        {           
        }
                
        //! \return name of the process.
        QString const getName() const
        {
            return m_name;   
        }
        
        //! Set pid.
        //! \param pid process id.
        void setPid( Q_PID const & pid )
        {
            m_pid = pid;
        }    
        //! \return the pid of the process.
        Q_PID getPid() const
        {
            return m_pid;   
        }
        
        //! Assign operator.
        //! \param other object to be assigned to this.
        ProcessData &operator=(ProcessData const & other)
        {
            if ( this == &other ){
                return *this;
            }
            m_name = other.m_name;
            m_pid  = other.m_pid;
            return *this;
        }
                
      private:
        //! Process name        
        QString   m_name;
        
        //! Process id
        Q_PID     m_pid;     
    };
    
    
        //! ProcessManager is a sigleton class for process management in theQTablet framework.
	
	//! 
	//!
	//! $Author: zchydem $
	//! $Date: 2008-04-13 23:34:11 +0300 (Sun, 13 Apr 2008) $
	//! $Revision: 84 $
        class ProcessManager: public QObject
    {
        Q_OBJECT
    public:
        //! Get the singleton instance of ProcessManager.
        //! \return instance of ProcessManager
        static ProcessManager * instance();
              
        //! Execute program \p program and the arguments can be given in the same string
        //! \param program e.g. "/usb/bin/ls -la "
        void exec( QString const & program );
       
        
    protected:
        //! CTor. Used internally
        ProcessManager();
        
        //! Dtor. Does nothing. 
        virtual ~ProcessManager();
        
    protected slots:
        //! Callback which is called when one of the previously started processes finish.
        //! \param exitCode   see Qt Documentation
        //! \param exitStatus see Qt Documentation        
        void finished( int exitCode, QProcess::ExitStatus exitStatus );
        
        //! Callback which is called on error.
        //! \param error see Qt Documentation 
        void error( QProcess::ProcessError error );
        
        //! Callback which is called when a process has started. After this slot
        //! is being called, the process is added to the process map.
        void started();
        
    signals:
        //! Signal for indicating process crash.
        //! \param pid process id
        //! \param name process name
        void processCrashed( qint64 pid, QString const name );
        
        //! Signal for indicating process error.
        //! \param error process error code.
        void processError( QProcess::ProcessError error );
        
        //! Signal for indicating that a process has been started
        //! \param processData information about the process (pid, name)
        void processStarted( ProcessData const processData );

        void processFinished( ProcessData const processData );
                           
    private:
        
        //! QMap for processes and their names
        QMap <QProcess*, ProcessData> m_processMap;
    };
}
#endif
