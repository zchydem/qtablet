#ifndef COMCHANNEL_HH
#define COMCHANNEL_HH

#include <QObject>
#include <QByteArray>

class QString;
class ComChannelPrivate;

namespace qtablet{

  //! ComChannel is a QTablet communication object, which can be used for IPC.
  //
  //! ComChannel provides pretty much the same functionality thatn QCopChannel
  //! without any dependecies to QWSServer in Qt Embedded. ComChannel requires
  //! one server instance to which all the other ComChannel instances connect.
  //! The ComChannel provice many-to-many communication channel so all the peers
  //! will receive all the messages that are send to specific channel. It is up
  //! to the peer to setup message content.
  //!
  //! \code
  //! Example
  //! ComChannel * channel = new ComChannel("NewChannel");
  //! channel->send("Message 1");
  //! \endcode
  //!
  class ComChannel: public QObject{
    Q_OBJECT
  public:
    //! Constructor which creates a new channel with the \p channelName if there isn't one yet.
    //! Use the default constructor in order to create the server object. This constructor creates
    //! a single peer.
    //!
    //! \param channelName The name of the new or existing channel
    //! \param parent Optional parent object
    ComChannel( QString const & channelName, QObject * parent = 0 );

    //! Constructor for creating the communication server object. This is required to be created
    //! in order to make all the peers work.
    ComChannel( QObject * parent = 0 );

    //! Destructor. If Destructor is called and there isn't any peers left in the channel, then
    //! channel will be deleted.
    virtual ~ComChannel();
			 
  public slots:
    //! Send \p message to the communication channel. The message may also contain \p data.
    //!
    //! \param message Message string
    //! \param data Message data
    void sendMessage( QString const & message, QByteArray const & data = QByteArray() );

  signals:
    //! Signal for indicating received \p message which may contain \p data.
    //!
    //! \param message Message string
    //! \param data Message data
    void messageReceived( QString const & message, QByteArray const & data );

    //! Signal for indicating channel creation.
    //! 
    //! \param newChannelName The Name of the new channel.
    void channelCreated( QString const & newChannelName );
    
    //! Signal for indicating channel deletion.
    //!
    //! \param deletedChannelName The name of the deleted channel
    void channelDeleted( QString const & deletedChannelName );

  private:    
    //! Private member
    ComChannelPrivate * d_ptr;  
    Q_DISABLE_COPY( ComChannel );
  };
}
#endif 
