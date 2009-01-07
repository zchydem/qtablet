#include <processmanager.hh>
#include <QProcess>
#include <QDebug>

namespace qtablet
{
    ProcessManager * ProcessManager::instance()
    {
        static ProcessManager instance;
        return &instance;
    }
   
    
    void ProcessManager::exec( QString const & program )
    {
        if ( program.isEmpty() )
        {
            return;   
        }
        
        QProcess *proc = new QProcess( this);
        
        
        connect( proc, SIGNAL( finished(int, QProcess::ExitStatus ) ),
                 this, SLOT  ( finished(int, QProcess::ExitStatus ) ) );
        connect( proc, SIGNAL( error( QProcess::ProcessError ) ),
                 this, SLOT  ( error( QProcess::ProcessError ) ) );
        connect( proc, SIGNAL( started() ),
                 this, SLOT  ( started() ) );
        
        qDebug() << "ProcessManager:exec: " << program;
        proc->start( program );     
        m_processMap.insert( proc,  ProcessData(0, program) );
    }


    ProcessManager::ProcessManager()
    {

    }
         
    ProcessManager::~ProcessManager()
    {

    }

    void ProcessManager::finished( int exitCode, QProcess::ExitStatus exitStatus )
    {
        Q_UNUSED( exitCode );
        
        // Remove the finished process from the map
        QProcess*    proc = qobject_cast<QProcess*>( sender() );
        ProcessData data = m_processMap.take( proc );
        
        if ( QProcess::CrashExit == exitStatus )
        {
            // Notify the framework about the crashed process
            emit processCrashed( data.getPid(), data.getName() );   
        }   
                
        qDebug() << "ProcessManager: process " << data.getName() << " with pid: " << data.getPid() << "finished.";
        
        // Let Qt handle process deletion
        proc->deleteLater();
        
        // Let the world know what this process has finished
        emit processFinished( data );
    }
        
     
    void ProcessManager::error( QProcess::ProcessError error )
    {
        // just forward the error. finished should handle process removal.
        emit processError( error );
    }
        
    void ProcessManager::started()
    {
        QProcess* proc = qobject_cast<QProcess*>( sender() );        
        
        // Get the ProcessData object from the map and set it's pid
        QMap<QProcess*, ProcessData>::iterator it = m_processMap.find( proc );
        
        if ( it == m_processMap.end() ){
            qCritical() << "ProcessManager: could not find the process from the processMap";
            return;   
        }
        
        // Now that we are sure that process is ok, let's store its pid
        it->setPid( proc->pid() );
        
        // let the interested parties to know that some process has started
        emit processStarted( *it );
    }
}
