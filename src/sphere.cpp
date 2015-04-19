/*
 * nodecast using Qt5 and libtorrent.
 * Copyright (C) 2014-2015  Frederic Logier
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * Contact : fred@pcode.io
 */


#include "sphere.h"

int Sphere::index = 0;

Sphere::Sphere(Sphere_data data, QStackedWidget *parent)
        : QAbstractButton(parent), sphere_data(data)
{

//    QPalette pal = palette();
//    pal.setColor(QPalette::Background, Qt::black);
//    setPalette(pal);
//    setAutoFillBackground(true);

  //  data.title.isEmpty()? m_title = data.title : m_title = "";

   // content = new QWidget(parent);

    qDebug() << "SPHERE DIRECTORY : " << sphere_data.directory;

    QString uuid;
    QDir nodecast_datas;
    QDir check_dir;

    switch(sphere_data.scope)
    {
    case Sphere_scope::PRIVATE :
        // check directory exist
        if (sphere_data.directory.isEmpty())
        {
            uuid = QUuid::createUuid().toString().remove("-").replace(0, 1, "");
            uuid = uuid.replace(uuid.size()-1, 1, "");
            sphere_data.directory = sphere_data.title.toLower().trimmed() + "_" + uuid;
        }

        nodecast_datas = prefs.getSavePath() + "/nodecast/spheres/private/";
        check_dir = nodecast_datas.absolutePath() + "/" + sphere_data.directory;
        if (!check_dir.exists())
            nodecast_datas.mkdir(sphere_data.directory);



        m_color = new QColor(105,105,105);

        media_scroll = new QScrollArea();
        media_scroll->setWidgetResizable(true);
        media_scroll->setFrameStyle(QFrame::NoFrame);
        media_container = new QWidget(media_scroll);
        flowLayout = new FlowLayout(media_container);

        media_scroll->setWidget(media_container);
        index_tab = parent->addWidget(media_scroll);
        qDebug() << "INDEX TAB : " << index_tab;

        break;

    case Sphere_scope::PUBLIC :
        m_color = new QColor(119,136,153);

        media_scroll = new QScrollArea();
        media_scroll->setWidgetResizable(true);
        media_scroll->setFrameStyle(QFrame::NoFrame);
        media_container = new QWidget(media_scroll);
        flowLayout = new FlowLayout(media_container);

        media_scroll->setWidget(media_container);

        view = new QWebView();
        view->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        view->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
        view->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
        view->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
        view->settings()->setAttribute(QWebSettings::NotificationsEnabled, false);
        view->setZoomFactor(0.8);
        view->load(QUrl(sphere_data.url));

        hSplitter = new QSplitter(Qt::Vertical);
        hSplitter->addWidget(view);
        hSplitter->addWidget(media_scroll);

        index_tab = parent->addWidget(hSplitter);
        qDebug() << "INDEX TAB : " << index_tab;

        break;

    case Sphere_scope::FIXED :
        m_color = new QColor(100, 143, 000);
        view = new QWebView();
        view->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        view->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
        view->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
        view->settings()->setAttribute(QWebSettings::NotificationsEnabled, false);
        view->setZoomFactor(0.8);
        view->load(QUrl(sphere_data.url));
        index_tab = parent->addWidget(view);

        //content = new QWidget();
        //index_tab = parent->addWidget(content);
        qDebug() << "INDEX TAB : " << index_tab;

        break;
    }


//    if (scope.compare("private") == 0)
//        m_color = new QColor(105,105,105);
//    else if (scope.compare("public") == 0)
//        m_color = new QColor(119,136,153);
//    else
//        m_color = new QColor(100, 143, 000);


    setAcceptDrops(true);
    setAutoExclusive(true);
    setCheckable(true);

    m_index = index++;

    connect(this, SIGNAL(clicked()), this, SLOT(selected()));
//    initStyleOption(QStyleOptionButton::Flat);
}

bool Sphere::isScopeFixed()
{
    bool scope = (sphere_data.scope == Sphere_scope::FIXED)? true : false;
    return scope;
}


void Sphere::reloadWeb()
{
    qDebug() << "RELOAD WEB";
    view->load(QUrl(sphere_data.url));
}


QString Sphere::get_directory()
{
    return sphere_data.directory;
}



QSize Sphere::sizeHint() const
{
    return QSize(105,105);
}



Sphere::~Sphere()
{}


void Sphere::dragEnterEvent(QDragEnterEvent *event)
{
    if (sphere_data.scope == Sphere_scope::FIXED) return;

    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void Sphere::dropEvent(QDropEvent* event)
{

    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();

    for(int i = 0; i < droppedUrlCnt; i++)
    {
       QString localPath = droppedUrls[i].toLocalFile();
       QFileInfo fileInfo(localPath);


       QString target_link = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory + "/" + fileInfo.fileName();
       fsutils::forceRemove(target_link);
       QFileInfo fileInfoLink(target_link);

       bool check_link = fsutils::createLink(localPath, fileInfoLink.absoluteFilePath());
       QString link_status = check_link? "LINK IS OK" : "LINK IS KO";
       qDebug() << link_status;
       if (!check_link)
       {
           QMessageBox::information(this, tr("Create link failed in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
           return;
       }

       if(fileInfo.isFile())
       {
           // file
           createTorrentDlg = new TorrentCreatorDlg(sphere_data.title, fileInfoLink.fileName(), fileInfoLink.absoluteFilePath(), this);
           connect(createTorrentDlg, SIGNAL(torrent_to_seed(QString)), this, SLOT(addTorrent(QString)));
           //QMessageBox::information(this, tr("Dropped file in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
       }
       else if(fileInfo.isDir())
       {
           // directory
           createTorrentDlg = new TorrentCreatorDlg(sphere_data.title, fileInfoLink.fileName(), fileInfoLink.absoluteFilePath(), this);
           connect(createTorrentDlg, SIGNAL(torrent_to_seed(QString)), this, SLOT(addTorrent(QString)));
           //QMessageBox::information(this, tr("Dropped directory in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
       }
       else
       {
           // none
           QMessageBox::information(this, tr("Dropped, but unknown in %1 sphere").arg(sphere_data.title), tr("Unknown: %1").arg(fileInfo.absoluteFilePath()));
       }
    }

    event->acceptProposedAction();
}



void Sphere::addTorrent(QString path)
{
  QBtSession::instance()->addTorrent(path);
}


void Sphere::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
//    QString css = "background-color: transparent; border: none; color: white";
//    setStyleSheet(css);


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);


    QColor color = this->isChecked() ? QColor(192,192,192) : *m_color;


    //painter.setPen( QColor("#222326") );
    //painter.setOpacity(1.0);
    //painter.setPen( Qt::TransparentMode );
    painter.setBrush(color);
    painter.drawEllipse(0, 0, 100, 100);
    //painter.setPen( Qt::SolidLine );

    //painter.drawText(10, 50, m_title);

    //painter.drawText(60, 70, m_title);

    QRectF rect(10, 10, 80, 90);
    painter.drawText(rect, Qt::AlignCenter, sphere_data.title);



//    painter.drawStaticText(10, 50, QStaticText("<center>" + m_title + "</center>"));


  //  setScaledContents(false);
}


void Sphere::selected()
{
    this->setChecked(true);
    //this->update();
    qDebug() << "selected : " << sphere_data.title << " index : " << m_index;
    emit row(m_index);
}

//void Sphere::mousePressEvent(QMouseEvent *e)
//{
//    mFirstX=0;
//    mFirstY=0;
////    mFirstClick=true;
////    mpaintflag=false;


//    if(e->button() == Qt::LeftButton)
//    {
//        qDebug() << "SPHERE CLICKED";
//        m_color = QColor(105,105,105);
//        this->setChecked(true);
//        //this->update();

//                //store 1st point
//              //  if(mFirstClick)
//              //  {
//                    mFirstX = e->x();
//                    mFirstY = e->y();
//                  //  mFirstClick = false;
//                  //  mpaintflag = true;
//                    qDebug() << "First image's coordinates" << mFirstX << "," << mFirstY << " title " << m_title;
//                  //  update();

//                //}

//            }
//}



void Sphere::addTorrent(const QTorrentHandle &h)
{
    // active sphere on new torrent
    this->setChecked(true);

    Widgettorrent *wt = new Widgettorrent(sphere_data);
    wt->addTorrent(h);
    flowLayout->addWidget(wt);
}



void Sphere::populate()
{
    qDebug() << "Sphere::polulate";
    Widgettorrent::populate(sphere_data, flowLayout);
}