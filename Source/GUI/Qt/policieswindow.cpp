/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "policieswindow.h"
#include "mainwindow.h"

#include "WebPage.h"
#include "WebView.h"
#include "progressbar.h"

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

PoliciesWindow::PoliciesWindow(MainWindow *parent) : CommonWebWindow(parent)
{
}

PoliciesWindow::~PoliciesWindow()
{
}

//***************************************************************************
// Visual elements
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesWindow::display_policies()
{
    display_html();
}

int PoliciesWindow::add_new_policies(const QStringList& files)
{
    WebPage* page = (WebPage*)main_window->web_view->page();
    if (!page)
        return -1;

    return page->import_policy(files);
}

//***************************************************************************
// WEB
//***************************************************************************

//---------------------------------------------------------------------------
void PoliciesWindow::change_qt_scripts_in_template(QString& html)
{
    QRegExp reg("\\{\\{ QT_SCRIPTS \\}\\}");
    QString script;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyWebKit.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/users/userWebKit.js\"></script>\n";
#elif defined(WEB_MACHINE_ENGINE)
    script += "        <script type=\"text/javascript\" src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/webengine.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/policyWebEngine.js\"></script>\n"
              "        <script type=\"text/javascript\" src=\"qrc:/users/userWebEngine.js\"></script>\n";
#endif
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyTree.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyTreeAffix.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyTreePolicies.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyTreeRules.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyTreeRulesMI.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyTreeRulesMT.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policyUser.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/policy.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/functions.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/utils/url.js\"></script>\n";
    script += "        <script type=\"text/javascript\" src=\"qrc:/menu.js\"></script>\n";
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), script);
}

//---------------------------------------------------------------------------
void PoliciesWindow::set_webmachine_script_in_template(QString& html)
{
    QRegExp reg("\\{\\{[\\s]+webmachine[\\s]\\}\\}");
    QString machine;
    int     pos = 0;

    reg.setMinimal(true);
#if defined(WEB_MACHINE_KIT)
    machine = "WEB_MACHINE_KIT";
#elif defined(WEB_MACHINE_ENGINE)
    machine = "WEB_MACHINE_ENGINE";
#endif
    if ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), machine);
}

//---------------------------------------------------------------------------
void PoliciesWindow::remove_result_in_template(QString& html)
{
    QRegExp reg("\\{% block result %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), "");
}

//---------------------------------------------------------------------------
void PoliciesWindow::change_checker_in_template(const QString& policy, QString& html)
{
    QRegExp reg("\\{% block checker %\\}\\{% endblock %\\}");
    int pos = 0;

    reg.setMinimal(true);
    while ((pos = reg.indexIn(html, pos)) != -1)
        html.replace(pos, reg.matchedLength(), policy);
}

//---------------------------------------------------------------------------
void PoliciesWindow::create_html_policy(QString& policy)
{
    QFile template_html(":/xsltPolicy.html");
    template_html.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray html = template_html.readAll();
    template_html.close();

    policy = QString(html);
}

//---------------------------------------------------------------------------
void PoliciesWindow::create_html_base(const QString& policy, QString& base)
{
    set_webmachine_script_in_template(base);
    change_qt_scripts_in_template(base);
    change_checker_in_template(policy, base);
    remove_result_in_template(base);
}

//---------------------------------------------------------------------------
void PoliciesWindow::create_html(QString &html)
{
    QString policy;
    create_html_policy(policy);
    create_html_base(policy, html);
}

}
