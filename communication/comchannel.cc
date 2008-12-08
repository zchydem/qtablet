#include "comchannel.hh"

namespace qtablet{

  ComChannel::ComChannel( QString const & channelName, QObject * parent ):
    QObject( parent ){

  }

  
  ComChannel::ComChannel( QObject * parent ):
    QObject( parent ){

  }

  ComChannel::~ComChannel(){

  }
			 
  void sendMessage( QString const & message, QByteArray const & data ){

  }

}
