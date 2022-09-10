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
        margin-left: 4px;
        margin-bottom: 0px;
        margin-top: 4px;
      }

      .mc {
        width: 100%;
        max-width: 1280px;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
        margin-top: 6px;
      }

      .mc p {
        margin: 0px;
        background-color: #64A8DD;
        font-weight: 700;
      }

      .mc span {
        color: black;
      }

      div .mc_element {
        padding-left: 1px;
      }

      .mc_rule {
        padding-left: 30px;
      }

      .arrow {
        display: inline-block;
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
        -webkit-transform: rotate(-45deg);
        -moz-transform: rotate(-45deg);
        -o-transform: rotate(-45deg); 
        transform: rotate(-45deg);
        margin-top:6px;
        margin-right:6px;
        margin-left:6px;
      }

      .arrow[type=checkbox]:checked ~ .extra {
        display: block;
      }

      .arrow:checked + label {
        display: inline-block;
        -webkit-transform: rotate(45deg);
        -moz-transform: rotate(45deg);
        -o-transform: rotate(45deg); 
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
        padding-left: 25px;
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
        <xsl:if test="@compare">
          <p>Compared to: <xsl:value-of select="@compare"/></p>
        </xsl:if>
      </div>
      <xsl:for-each select="mc:implementationChecks">
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
                  <xsl:text>  |  </xsl:text>
                </xsl:if>
                <xsl:text>Results: </xsl:text>
                <xsl:choose>
                  <xsl:when test="@fail_count &gt; 0">
                    <xsl:text>&#x274C;</xsl:text><xsl:value-of select="@outcome"/>
                  </xsl:when>
                  <xsl:when test="@warn_count &gt; 0">
                    <xsl:text>&#x26A0;</xsl:text><xsl:value-of select="@outcome"/>
                  </xsl:when>
                  <xsl:when test="@info_count &gt; 0">
                    <xsl:text>&#x2139;</xsl:text><xsl:value-of select="@outcome"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>&#x2705;</xsl:text><xsl:value-of select="@outcome"/>
                  </xsl:otherwise>
                </xsl:choose>
                  <xsl:if test="@fail_count &gt; 0">
                    <xsl:text>  |  </xsl:text>
                    <xsl:text>Fail count: </xsl:text><xsl:value-of select="@fail_count"/>
                  </xsl:if>
                  <xsl:if test="@warn_count &gt; 0">
                    <xsl:text>  |  </xsl:text>
                    <xsl:text>Warn count: </xsl:text><xsl:value-of select="@warn_count"/>
                  </xsl:if>
                  <xsl:if test="@info_count &gt; 0">
                    <xsl:text>  |  </xsl:text>
                    <xsl:text>Info count: </xsl:text><xsl:value-of select="@info_count"/>
                  </xsl:if>
              <xsl:for-each select="mc:test">
                <div class="extra">
                  <xsl:if test="../@name">
                    <strong><xsl:text>Name: </xsl:text></strong> <xsl:value-of select="../@name"/>
                    <br/>
                  </xsl:if>
                  <strong><xsl:value-of select="/mc:context/@name"/></strong>
                  <strong><xsl:text>Results: </xsl:text></strong> 
                  <xsl:value-of select="@outcome"/>
                  <xsl:if test="@outcome = 'pass'">
                    <xsl:text> &#x2705;  </xsl:text>
                  </xsl:if>
                  <xsl:if test="@outcome = 'info'">
                    <xsl:text> &#x2139;  </xsl:text>
                    <xsl:if test="@reason">
                      <xsl:text>(Reason: </xsl:text>
                      <xsl:value-of select="@reason"/>
                      <xsl:text>)</xsl:text>
                    </xsl:if>
                  </xsl:if>
                  <xsl:if test="@outcome = 'warn'">
                    <xsl:text> &#x26A0;  </xsl:text>
                    <xsl:if test="@reason">
                      <xsl:text>(Reason: </xsl:text>
                      <xsl:value-of select="@reason"/>
                      <xsl:text>)</xsl:text>
                    </xsl:if>
                  </xsl:if>
                  <xsl:if test="@outcome = 'fail'">
                    <xsl:text> &#x274C;  </xsl:text>
                    <xsl:if test="@reason">
                      <xsl:text>(Reason: </xsl:text>
                      <xsl:value-of select="@reason"/>
                      <xsl:text>)</xsl:text>
                    </xsl:if>
                  </xsl:if>
                  <xsl:if test="@outcome = 'n/a'">
                    <xsl:if test="@reason">
                      <xsl:text>(Reason: </xsl:text>
                      <xsl:value-of select="@reason"/>
                      <xsl:text>)</xsl:text>
                    </xsl:if>
                  </xsl:if>
                  <br/>
                  <xsl:for-each select="../mc:context">
                    <strong><xsl:value-of select="@name"/></strong>
                    <xsl:text>: </xsl:text>
                    <xsl:value-of select="."/><br/>
                 </xsl:for-each>
                 <xsl:for-each select="mc:value">
                   <xsl:if test="@name !=''">
                     <strong>Name: </strong><xsl:value-of select="@name"/><br/>
                   </xsl:if>
                   <xsl:if test="@offset !=''">
                     <strong>Offset: </strong><xsl:value-of select="@offset"/><br/>
                   </xsl:if>
                   <xsl:if test="@formatid !=''">
                     <strong>ID: </strong><xsl:value-of select="@formatid"/><br/>
                   </xsl:if>
                   <xsl:if test="@context !=''">
                     <strong>Value context: </strong><xsl:value-of select="@context"/><br/>
                   </xsl:if>
                   <xsl:if test=". !=''">
                     <strong>Value: </strong><xsl:value-of select="."/><br/>
                   </xsl:if>
                 </xsl:for-each>
                   <hr/>
                  </div> 
                </xsl:for-each>
              </td>
            </tr>
          </xsl:for-each> 
        </table>
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
          <xsl:when test="@outcome = 'warn'">
            <xsl:text>  &#x26A0; warn</xsl:text>
          </xsl:when>
          <xsl:when test="@outcome = 'info'">
            <xsl:text>  &#x2139; info</xsl:text>
          </xsl:when>
          <xsl:when test="@outcome = 'pass'">
            <xsl:text>  &#x2705; pass</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>  N/A</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </h2>
      <div class="mc_rule extra">
      <xsl:if test="mc:description != ''">
        <p class="description">
          <xsl:value-of select="mc:description"/>
        </p>
      </xsl:if>
      <p>
      <strong>Type:</strong><xsl:text> </xsl:text><xsl:value-of select="@type"/>
      <xsl:if test="@level != ''">
        | <strong>Level: </strong>  <xsl:value-of select="@level"/>
      </xsl:if>
      | <strong>Rules run:</strong><xsl:text> </xsl:text><xsl:value-of select="@rules_run"/>
      | <strong>Fail count:</strong><xsl:text> </xsl:text><xsl:value-of select="@fail_count"/>
      | <strong>Warn count:</strong><xsl:text> </xsl:text><xsl:value-of select="@warn_count"/>
      | <strong>Info count:</strong><xsl:text> </xsl:text><xsl:value-of select="@info_count"/>
      | <strong>Pass count:</strong><xsl:text> </xsl:text><xsl:value-of select="@pass_count"/></p> 
        <xsl:apply-templates select="mc:rule|mc:policy"/>
      </div>
    </div>
  </xsl:template>

  <xsl:template match="mc:rule">
    <div class="mc">
      <div class="mc_element">
      <input id="policy-arrow-{generate-id()}" class="arrow" type="checkbox"/>
      <label for="policy-arrow-{generate-id()}"></label>
      <xsl:text> </xsl:text>
      <strong>
        <span><xsl:value-of select="@name"/></span>
      </strong>
      <xsl:text> </xsl:text>
      <xsl:if test="@outcome = 'pass'">
        <xsl:text>&#x2705;  </xsl:text>
        <xsl:value-of select="@outcome"/>
      </xsl:if>
      <xsl:if test="@outcome = 'info'">
        <xsl:text>&#x2139;  </xsl:text>
        <xsl:value-of select="@outcome"/>
      </xsl:if>
      <xsl:if test="@outcome = 'warn'">
        <xsl:text>&#x26A0;  </xsl:text>
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
        <xsl:if test="mc:description != ''">
          <p class="description">
            <xsl:value-of select="mc:description"/>
          </p>
        </xsl:if>
        <xsl:if test="@value != ''">
          <strong>Value: </strong>  <xsl:value-of select="@value"/>
          <br/>
        </xsl:if>
        <xsl:if test="@tracktype != ''">
          <strong>Tracktype: </strong>  <xsl:value-of select="@tracktype"/>
          <br/>
        </xsl:if>
        <xsl:if test="@occurence != ''">
          <strong>Occurence: </strong>  <xsl:value-of select="@occurence"/>
          <br/>
        </xsl:if>
        <xsl:if test="@operator != ''">
          <strong>Operator: </strong>  <xsl:value-of select="@operator"/>
          <br/>
        </xsl:if>
        <xsl:if test="@requested != ''">
          <strong>Requested: </strong>  <xsl:value-of select="@requested"/>
          <br/>
        </xsl:if>
        <xsl:if test="@actual != ''">
          <strong>Actual: </strong>  <xsl:value-of select="@actual"/>
          <br/>
        </xsl:if>
        <xsl:if test="@compared_to != ''">
          <strong>Compared to: </strong>  <xsl:value-of select="@compared_to"/>
          <br/>
        </xsl:if>
        <xsl:if test="@level != ''">
          <strong>Level: </strong>  <xsl:value-of select="@level"/>
          <br/>
        </xsl:if>
        <strong><xsl:text>Xpath:  </xsl:text></strong>
        <xsl:value-of select="@xpath"/>
      </div>
    </div>
  </div>
  </xsl:template>
</xsl:stylesheet>
