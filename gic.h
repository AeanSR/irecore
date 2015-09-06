/*
    Copyright (C) 2015 Aean(a.k.a. fhsvengetta)
    All rights reserved.

    IreCore is distributed under the terms of The MIT License.
    You should have received a copy of the MIT License along with this program.
    If not, see <http://opensource.org/licenses/mit-license.php>.
*/
#ifndef GIC_H
#define GIC_H

#include <QtWidgets/QMainWindow>
#include <QtConcurrent/QtConcurrent>
#include "ui_gic.h"
#include <QtNetwork>
#include <qthread.h>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QtWidgets>
#include "rapidjson\document.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\prettywriter.h"

extern int silence_mode;

class gic : public QMainWindow
{
    Q_OBJECT

public:
    gic( QWidget* parent = 0 );
    ~gic();
    void reset_result_page();
    void run_simulation();
    void set_arguments();
    int import_player( std::string& realm, std::string& name, std::string& region, int silence = 0 );
    //int retrive_item_subclass(int id, std::string& region);
	void run_scripts();
public slots:
    void on_btnRun_clicked();
    void on_btnImport_clicked();
    void on_btnResetBuild_clicked();
    void on_btnGenerateDefaultAPL_clicked();
    void on_listActions_itemDoubleClicked();
    void on_listConditions_itemDoubleClicked();
    void on_comboIncandescence_currentIndexChanged( int );
    void TxtBoxNotify( QString );
    void mh_dps_calculate();
    void oh_dps_calculate();
    void gear_summary_calculate();
    void select_gear_slot();
private:
    Ui::gicClass ui;

};

class functionbuf
        : public std::streambuf {
private:
    typedef std::streambuf::traits_type traits_type;
    char          d_buffer[102400];
    gic* pg;
    int overflow( int c ) {
        if ( !traits_type::eq_int_type( c, traits_type::eof() ) ) {
            *this->pptr() = traits_type::to_char_type( c );
            this->pbump( 1 );
        }
        return this->sync() ? traits_type::not_eof( c ) : traits_type::eof();
    }
    int sync() {
        if ( this->pbase() != this->pptr() ) {
			if (!silence_mode)
            QMetaObject::invokeMethod( pg, "TxtBoxNotify", Q_ARG( QString, QString( std::string( this->pbase(), this->pptr() ).c_str() ) ) );
            this->setp( this->pbase(), this->epptr() );
        }
        return 0;
    }
public:
    functionbuf( gic* pg ) : pg( pg )
    {
        this->setp( this->d_buffer, this->d_buffer + sizeof( this->d_buffer ) - 1 );
    }
};

class ofunctionstream
    : private virtual functionbuf
        , public std::ostream {
public:
    ofunctionstream( gic* pg )
        : functionbuf( pg ), std::ostream( static_cast<std::streambuf*>( this ) ) {
        this->flags( std::ios_base::unitbuf );
    }
};

extern gic* global_pgic;
extern ofunctionstream* simlog;

class getjson {

private:
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply* reply;
    gic* pg;
public:
    void set_url( QString& surl ) {
        url.setUrl( surl, QUrl::ParsingMode::TolerantMode );
    }
    void set_parent( gic* _pg ) {
        pg = _pg;
    }
    rapidjson::Document get();
};


#endif // GIC_H
