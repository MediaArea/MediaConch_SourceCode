<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:x="http://www.w3.org/1999/xhtml" version="1.0" extension-element-prefixes="xsi" exclude-result-prefixes="x" >
  <xsl:output encoding="UTF-8" method="html" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
  <html>
  <head>
    <title>MediaConch</title>
    <style>
      .mc_header {
        width: 600px;
        border: 1px black solid;
        background-color: #64A8DD;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 14px;
      }

      .mc_header h4, .mc_header div {
        margin: 10px;
        text-align: center;
        width: 600px;
      }

      .mc {
        width: 600px;
        background-color: #F2F5F8;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
      }

      .mc h1  {
        background-color: #64A8DD;
      }

      .mc table {
        border-collapse:collapse;
        border: 1px black solid;
        width   : 600px;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
      }

      .mc td .reason {
        border-bottom: thin solid black;
      }

      .mc tr {
        border-bottom: 1px black solid;
      }

      .mc p {
        margin: 0px;
        background-color: #64A8DD;
        font-size: 12px;
        font-weight: 700;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
      }

      .cmn-toggle {
        position: absolute;
        margin-left: -9999px;
        visibility: hidden;
      }
      .cmn-toggle + label {
        display: block;
        position: relative;
        cursor: pointer;
        outline: none;
        user-select: none;
      }

      input.cmn-toggle-round + label {
        padding: 2px;
        width: 30px;
        height: 15px;
        background-color: #dddddd;
        border-radius: 15px;
      }

      input.cmn-toggle-round + label:before,
      input.cmn-toggle-round + label:after {
        display: block;
        position: absolute;
        top: 1px;
        left: 1px;
        bottom: 1px;
        content: "";
      }
      input.cmn-toggle-round + label:before {
        right: 1px;
        background-color: #f1f1f1;
        border-radius: 28px;
        transition: background 0.4s;
      }
      input.cmn-toggle-round + label:after {
        width: 14px;
        background-color: #fff;
        border-radius: 100%;
        box-shadow: 0 2px 5px rgba(0, 0, 0, 0.3);
        transition: margin 0.4s;
      }
      input.cmn-toggle-round:checked + label:before {
        background-color: #8ce196;
      }
      input.cmn-toggle-round:checked + label:after {
        margin-left: 15px;
      }

      input[type=checkbox]:checked ~ .mc .extra {
         display: block;
      }

      .extra {
      display: none;
      }

      .verbosity {
        width: 600px;
        background-color: #F2F5F8;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
      }
    </style>
  </head>
  <body>
    <xsl:for-each select="mc:media">
      <div class="mc_header">
        <h4>&#x1F41A; MediaConch Report for <xsl:value-of select="@ref"/></h4>
      </div>
      <xsl:for-each select="mc:implementationChecks">
        <div class="mc_header">
          <h4>
            <xsl:value-of select="mc:name"/>
          </h4>
          <p>
            <xsl:value-of select="mc:description"/>
          </p>
        </div>
        <table class="mc"> 
          <xsl:if test="position() &lt; 2">
            <div class="verbosity mc_header">Verbose mode?</div>
            <input id="implementation-toggle" class="cmn-toggle cmn-toggle-round" type="checkbox"/>
            <label for="implementation-toggle"></label>
          </xsl:if>
            <xsl:for-each select="mc:check/mc:test">
              <td>
                <strong><xsl:value-of select="../@icid"/></strong>
                <xsl:text>:  </xsl:text>
                <strong><xsl:value-of select="../@name"/></strong>
                <xsl:text>  </xsl:text>
                <xsl:value-of select="@outcome"/>
                <xsl:text>  </xsl:text>
                <xsl:if test="@outcome = 'pass'">
                  <xsl:text>&#x2705;  </xsl:text>
                </xsl:if>
                <xsl:if test="@outcome = 'fail'">
                  <xsl:text>&#x274C;  </xsl:text>
                </xsl:if>
              </td>
                  <xsl:if test="@outcome = 'fail'">
                    <tr>
                      <td colspan="2" class="reason">
                        <strong>
                          <xsl:text>&#x274C; Reason: </xsl:text>
                        </strong>
                        <xsl:value-of select="@reason"/>
                      </td>
                    </tr>
                  </xsl:if>
                    <tr>
                      <td>
                        <div class="extra">
                         Context of test: <xsl:value-of select="../mc:context/@name"/>
                             <xsl:value-of select="../mc:context"/>
                      </div>
                      <xsl:for-each select="mc:value">
                        <div class="extra">
                         Name: <xsl:value-of select="@name"/><br/>
                         Offset: <xsl:value-of select="@offset"/><br/>
                         Value context: <xsl:value-of select="@context"/><br/>
                         ID: <xsl:value-of select="@id"/><br/>
                         Value: <xsl:value-of select="."/><br/>
                        <br/>
                        </div>
                      </xsl:for-each>
                    </td>
                  </tr>
            </xsl:for-each>
          </table>
      </xsl:for-each>
      <xsl:for-each select="mc:policyChecks">
        <div class="mc_header">
          <h4>
            <xsl:value-of select="mc:name"/>
          </h4>
          <p>
            <xsl:value-of select="mc:description"/>
          </p>
        </div>
        <table class="mc">
        <div class="verbosity">Verbose?</div>
        <input id="policy-toggle" class="cmn-toggle cmn-toggle-round" type="checkbox"/>
        <label for="policy-toggle"></label>
          <xsl:for-each select="mc:check/mc:test">
            <tr><td>
              <xsl:value-of select="../@name"/>
              <xsl:text>: </xsl:text>
                <xsl:if test="@outcome = 'pass'">
                  <xsl:text>&#x2705;  </xsl:text>
                </xsl:if>
                <xsl:if test="@outcome = 'fail'">
                  <xsl:text>&#x274C;  </xsl:text>
                </xsl:if>
                <xsl:value-of select="@outcome"/>
            </td></tr>
            <tr>
              <td class="extra">
                <xsl:if test="../mc:context/@field != ''">
                    <xsl:text>Context (field): </xsl:text>
                    <xsl:value-of select="../mc:context/@field"/><br/>
                </xsl:if>
                <xsl:if test="../mc:context/@value != ''">           
                    <xsl:text>Context (value): </xsl:text>
                    <xsl:value-of select="../mc:context/@value"/><br/>
                </xsl:if>
              </td>
            </tr>
            <xsl:if test="@outcome = 'fail'">
            <tr>
              <td>
               Reason: <xsl:value-of select="@reason"/>
              </td>
            </tr>
            </xsl:if>
            <tr>
              <td class="extra">
               Track type: <xsl:value-of select="@tracktype"/>
              </td>
            </tr><tr>
              <td class="extra">
              Track type order:  <xsl:value-of select="@tracktypeorder"/>
              </td>
            </tr><tr>
              <td class="extra">
              Track ID:  <xsl:value-of select="@trackid"/>
              </td>
            </tr><tr>
              <td class="extra">
              Actual:  <xsl:value-of select="@actual"/>
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </xsl:for-each>
    </xsl:for-each>
  </body>
  </html>
  </xsl:template>
</xsl:stylesheet>
