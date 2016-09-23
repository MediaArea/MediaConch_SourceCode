<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:x="http://www.w3.org/1999/xhtml" version="1.0" extension-element-prefixes="xsi" exclude-result-prefixes="mc x" >
  <xsl:output encoding="UTF-8" method="html" version="1.0" indent="yes" doctype-system="about:legacy-compat"/>
  <xsl:template match="/mc:MediaConch">
  <html>
  <head>
    <title>MediaConch</title>
    <style type="text/css">

      .mediaconch_report {
        max-width: 1280px;
        border: 1px black solid;
        padding-left: 10px;
        padding-right: 10px;
      }

      .mc_header {
        width: 100%;
        max-width: 1280px;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
      }

      .mc_header p {
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
        margin-bottom: 0;
        margin-top: 0;
      }

      .mc_header h1 {
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 18px;
      }

      .mc_header h2 {
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 14px;
        display: inline-block;
        margin-left: 6px;
        margin-bottom: 0px;
        margin-top: 4px;
      }

      .mc {
        width: 100%;
        max-width: 1280px;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
        margin-top: 12px;
      }

      .mc p {
        margin: 0px;
        background-color: #64A8DD;
        font-weight: 700;
      }

      div .mc_element {
        padding: 0 5px 0 30px;
      }

      .mc_rule {
        padding-left: 30px;
      }

      .arrow {
        position: absolute;
        margin-left: -9999px;
        visibility: hidden;
      }

      .arrow + label {
        display: inline-block;
        position: relative;
        cursor: pointer;
        outline: none;
        user-select: none;
        border-right:3px solid black;
        border-bottom:3px solid black;
        width:6px;
        height:6px;
        transform: rotate(-45deg);
        margin-top:6px;
        margin-right:6px;
        margin-left:6px;
      }

      .arrow[type=checkbox]:checked ~ .extra {
        display: block;
      }

      .arrow:checked + label {
        transform: rotate(45deg);
      }

      .toggle {
        position: absolute;
        margin-left: -9999px;
        visibility: hidden;
      }

      .toggle + label {
        display: inline-block;
        position: relative;
        cursor: pointer;
        outline: none;
        user-select: none;
      }

      input.toggle-round + label {
        padding: 2px;
        width: 30px;
        height: 12px;
        background-color: #dddddd;
        border-radius: 15px;
        vertical-align:middle;
      }

      input.toggle-round + label:before,
      input.toggle-round + label:after {
        display: inline-block;
        position: absolute;
        top: 1px;
        left: 1px;
        bottom: 1px;
        content: "";
      }

      input.toggle-round + label:before {
        right: 1px;
        background-color: #f1f1f1;
        border-radius: 28px;
        transition: background 0.4s;
      }

      input.toggle-round + label:after {
        width: 14px;
        background-color: #fff;
        border-radius: 100%;
        box-shadow: 0 2px 5px rgba(0, 0, 0, 0.3);
        transition: margin 0.4s;
      }

      input.toggle-round:checked + label:before {
        background-color: #8ce196;
      }

      input.toggle-round:checked + label:after {
        margin-left: 15px;
      }

      input.toggle-round[type=checkbox]:checked ~ .mc .extra {
        display: block;
      }

      input.toggle-round[type=checkbox]:not(:checked) ~ .extra {
        display: none;
      }

      .extra {
        display: none;
        padding-left: 20px;
      }

      .verbosity {
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
        margin-left:20px;
        padding-right:4px;
      }
    </style>
  </head>
  <body class="mediaconch_report">
    <xsl:for-each select="mc:media">
      <div class="mc_header">
        <h1>&#x1F41A; MediaConch Report</h1>
        <p>File: <xsl:value-of select="@ref"/></p>
      </div>
      <xsl:for-each select="mc:implementationChecks">
        <xsl:if test="@checks_run &gt; 0">
          <div class="mc_header">
            <h2>
              <xsl:value-of select="mc:name"/>
            </h2>
            <p>
              <xsl:value-of select="mc:description"/>
            </p>
          </div>
          <xsl:if test="position() &lt; 2">
            <span class="verbosity">Toggle all verbosity:  </span> 
            <input id="implementation-toggle-{generate-id()}" class="toggle toggle-round" type="checkbox"/>
            <label for="implementation-toggle-{generate-id()}"></label>
          </xsl:if>
          <table class="mc">
            <xsl:for-each select="mc:check">
              <tr>
                <td class="mc_element">
                  <input id="implementation-arrow-{generate-id()}" class="arrow" type="checkbox"/>
                  <label for="implementation-arrow-{generate-id()}"></label>
                  <strong><xsl:value-of select="@icid"/></strong>
                  <xsl:if test="@tests_run !=''">
                    <xsl:text>  Tests run: </xsl:text><xsl:value-of select="@tests_run"/>
                    <xsl:text>  | </xsl:text>
                  </xsl:if>
                  <xsl:text> Results: </xsl:text>
                  <xsl:choose>
                    <xsl:when test="@fail_count &gt; 0">
                      <xsl:text>&#x274C;  </xsl:text>
                      <xsl:text>  Fail count: </xsl:text><xsl:value-of select="@fail_count"/>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:text>&#x2705;  </xsl:text><xsl:value-of select="@outcome"/>
                    </xsl:otherwise>
                  </xsl:choose>
                <xsl:for-each select="mc:test">
                  <div class="extra">
                    <strong><xsl:value-of select="/mc:context/@name"/></strong>
                    <strong><xsl:text>Results: </xsl:text></strong> 
                    <xsl:value-of select="@outcome"/>
                    <xsl:if test="@outcome = 'pass'">
                      <xsl:text> &#x2705;  </xsl:text>
                    </xsl:if>
                    <xsl:if test="@outcome = 'fail'">
                      <xsl:text> &#x274C;  </xsl:text>
                      <xsl:text>(Reason: </xsl:text>
                      <xsl:value-of select="@reason"/>
                      <xsl:text>)</xsl:text>
                    </xsl:if>
                    <xsl:if test="@outcome = 'n/a'">
                    (Reason: <xsl:value-of select="@reason"/>
                    <xsl:text>) </xsl:text>
                    </xsl:if>
                    <br/>
                    <xsl:if test="../mc:context !=''">
                      <strong><xsl:value-of select="../mc:context/@name"/></strong>
                      <xsl:text>: </xsl:text>
                      <xsl:value-of select="../mc:context"/><br/>
                   </xsl:if>
                    <xsl:if test="mc:value/@name !=''">
                       <strong>Name: </strong><xsl:value-of select="mc:value/@name"/><br/>
                     </xsl:if>
                     <xsl:if test="mc:value/@offset !=''">
                       <strong>Offset: </strong><xsl:value-of select="mc:value/@offset"/><br/>
                     </xsl:if>
                     <xsl:if test="mc:value/@formatid !=''">
                       <strong>ID: </strong><xsl:value-of select="mc:value/@formatid"/><br/>
                     </xsl:if>
                     <xsl:if test="mc:value/@context !=''">
                       <strong>Value context: </strong><xsl:value-of select="mc:value/@context"/><br/>
                     </xsl:if>
                     <xsl:if test="mc:value/. !=''">
                       <strong>Value: </strong><xsl:value-of select="mc:value/."/><br/>
                     </xsl:if>
                    </div> 
                  </xsl:for-each>
                </td>
              </tr>
            </xsl:for-each> 
          </table>
        </xsl:if>
      </xsl:for-each>
      <xsl:apply-templates select="mc:rule|mc:policy"/>
    </xsl:for-each>
    <br/>
  </body>
  </html>
  </xsl:template>

  <xsl:template match="mc:policy">
    <div class="mc_header">
      <xsl:choose>
        <xsl:when test="name(parent::*) = 'media'">
          <input id="policy-arrow-{generate-id()}" class="arrow" type="checkbox" checked="checked"/>
        </xsl:when>
        <xsl:otherwise>
          <input id="policy-arrow-{generate-id()}" class="arrow" type="checkbox"/>
        </xsl:otherwise>
      </xsl:choose>
      <label for="policy-arrow-{generate-id()}"></label>
      <h2>
        <xsl:value-of select="@name"/>
        <xsl:choose>
          <xsl:when test="@outcome = 'fail'">
            <xsl:text>  &#x274C; fail</xsl:text>
            <xsl:if test="@actual != ''">
              (Actual:<xsl:value-of select="@actual"/>)
            </xsl:if>
          </xsl:when>
          <xsl:when test="@outcome = 'pass'">
            <xsl:text>  &#x2705; pass</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>  N/A</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </h2>
      <xsl:if test="mc:description != ''">
        <p>
          <xsl:value-of select="mc:description"/>
        </p>
      </xsl:if>
      <div class="mc_rule extra">
      <p>
      <strong>Type:</strong><xsl:text> </xsl:text><xsl:value-of select="@type"/>
      | <strong>Rules run:</strong><xsl:text> </xsl:text><xsl:value-of select="@rules_run"/>
      | <strong>Fail count:</strong><xsl:text> </xsl:text><xsl:value-of select="@fail_count"/>
      | <strong>Pass count:</strong><xsl:text> </xsl:text><xsl:value-of select="@pass_count"/></p> 
        <xsl:apply-templates select="mc:rule|mc:policy"/>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="mc:rule">
    <div class="mc">
      <input id="policy-arrow-{generate-id()}" class="arrow" type="checkbox"/>
      <label for="policy-arrow-{generate-id()}"></label>
      <xsl:text> </xsl:text>
      <strong>
        <xsl:value-of select="@name"/>
      </strong>
      <xsl:text> </xsl:text>
      <xsl:if test="@outcome = 'pass'">
        <xsl:text>&#x2705;  </xsl:text>
        <xsl:value-of select="@outcome"/>
      </xsl:if>
      <xsl:if test="@outcome = 'fail'">
        <xsl:text>&#x274C;  </xsl:text>
      <xsl:value-of select="@outcome"/>
      <xsl:if test="@actual != ''">
        (Actual: <xsl:value-of select="@actual"/>)
      </xsl:if>
      </xsl:if>
      <xsl:if test="@outcome = 'N/A'">
      <xsl:value-of select="@outcome"/>
      </xsl:if>
      <div class="extra">
        <xsl:if test="@actual != ''">
          <strong>Actual: </strong>  <xsl:value-of select="@actual"/>
          <br/>
        </xsl:if>
        <strong><xsl:text>Xpath:  </xsl:text></strong>
        <xsl:value-of select="@xpath"/>
      </div>
    </div>
  </xsl:template>
</xsl:stylesheet>
