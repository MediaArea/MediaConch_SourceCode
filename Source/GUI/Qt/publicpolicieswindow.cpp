/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "publicpolicieswindow.h"
#include "mainwindow.h"

#include "WebPage.h"
#include "WebView.h"
#include "progressbar.h"
#include <QProgressBar>
#include <QRegularExpression>

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PublicPoliciesWindow::PublicPoliciesWindow(MainWindow *parent) : CommonWebWindow(parent)
{
}

PublicPoliciesWindow::~PublicPoliciesWindow()
{
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void PublicPoliciesWindow::display_public_policies()
{
    display_html();
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::change_qt_scripts_in_template(QString& html)
{
    QRegularExpression reg("\\{\\{ QT_SCRIPTS \\}\\}", QRegularExpression::InvertedGreedinessOption);
    QString script;
    int     pos = 0;

#if defined(WEB_MACHINE_KIT)
    script += "        <script type=\"text/javascript\" src=\"qrc:/publicPolicies/listWebKit.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/users/userWebKit.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script += "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/publicPolicies/listWebEngine.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/users/userWebEngine.js\"></script>\n";
#endif
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/text.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/publicPolicies/list.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/publicPolicies/listPolicyTree.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/url.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/menu.js\"></script>\n";
    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
        html.replace(pos, match.capturedLength(), script);
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::set_webmachine_script_in_template(QString& html)
{
    QRegularExpression reg("\\{\\{[\\s]+webmachine[\\s]\\}\\}", QRegularExpression::InvertedGreedinessOption);
    QString machine;
    int     pos = 0;

#if defined(WEB_MACHINE_KIT)
    machine = "WEB_MACHINE_KIT";
#elif defined(WEB_MACHINE_ENGINE)
    machine = "WEB_MACHINE_ENGINE";
#endif
    QRegularExpressionMatch match = reg.match(html, pos);
    if ((pos = match.capturedStart()) != -1)
        html.replace(pos, match.capturedLength(), machine);
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::remove_result_in_template(QString& html)
{
    QRegularExpression reg("\\{% block result %\\}\\{% endblock %\\}", QRegularExpression::InvertedGreedinessOption);
    html.replace(reg, "");
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::change_checker_in_template(const QString& policy, QString& html)
{
    QRegularExpression reg("\\{% block checker %\\}\\{% endblock %\\}", QRegularExpression::InvertedGreedinessOption);
    html.replace(reg, policy);
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::create_html_policy(QString& policy)
{
    QFile template_html(":/publicPolicies.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    policy = QString(html);
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::create_html_base(const QString& policy, QString& base)
{
    set_webmachine_script_in_template(base);
    change_qt_scripts_in_template(base);
    change_checker_in_template(policy, base);
    remove_result_in_template(base);
}

//---------------------------------------------------------------------------
void PublicPoliciesWindow::create_html(QString &html)
{
    QString policy;
    create_html_policy(policy);
    create_html_base(policy, html);
}

//***************************************************************************
// HELPER
//***************************************************************************

}
