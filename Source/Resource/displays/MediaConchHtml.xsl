<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:x="http://www.w3.org/1999/xhtml" version="1.0" extension-element-prefixes="xsi" exclude-result-prefixes="x" >
  <xsl:output encoding="UTF-8" method="html" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
  <html>
  <head>
    <title>MediaConch</title>
    <style type="text/css">
      .mc_header {
        width: 100%;
        max-width: 1280px;
        border: 1px black solid;
        background-color: #64A8DD;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 14px;
      }

      .mc_header h4, .mc_header div {
        margin: 10px;
        text-align: center;
      }

      .mc_header p {
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
      }

      .mc {
        width: 100%;
        max-width: 1280px;
        background-color: #F2F5F8;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
        font-size: 12px;
      }

      .mc h1  {
        background-color: #64A8DD;
      }

      .mc hr  {
        width: 99%;
        border-top: 1px solid #000;
        margin-top: 10px;
        margin-bottom: 10px;
        box-sizing: content-box;
        height: 0;
      }

      .mc p {
        margin: 0px;
        background-color: #64A8DD;
        font-size: 12px;
        font-weight: 700;
        font-family: 'Open Sans', Helvetica, Arial, sans-serif;
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
        border-right:5px solid black;
        border-bottom:5px solid black;
        width:10px;
        height:10px;
        transform: rotate(-45deg);
        margin-top:5px;
        margin-right:5px;
      }

      .arrow input:checked + label {
        width: 0;
        height: 0;
        border-style: solid;
        border-width: 8px 0 8px 8px;
        border-color: transparent transparent black transparent;
      }

      input[type=checkbox]:checked ~ .extra {
        display: block;
      }

      input.arrow:checked + label {
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
        height: 11px;
        background-color: #dddddd;
        border-radius: 15px;
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

      input[type=checkbox]:checked ~ .mc .extra {
        display: block;
      }

      input[type=checkbox]:checked ~ .mc .arrow + label {
        display: none;
      }

      .extra {
        display: none;
      }

      .verbosity {
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
        <xsl:if test="position() &lt; 2">
          <span class="verbosity">Toggle all verbosity:  </span> 
          <input id="implementation-toggle-{generate-id()}" class="toggle toggle-round" type="checkbox"/>
          <label for="implementation-toggle-{generate-id()}"></label>
        </xsl:if>
        <table class="mc">
          <xsl:for-each select="mc:check/mc:test">
          <tr>
            <td>
              <input id="implementation-arrow-{generate-id()}" class="arrow arrow-mark" type="checkbox"/>
              <label for="implementation-arrow-{generate-id()}"></label>
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
                  <xsl:text>(Reason: </xsl:text>
                  <xsl:value-of select="@reason"/>
                  <xsl:text>)</xsl:text>
                </xsl:if>
                <xsl:if test="@outcome = 'n/a'">
                (Reason: <xsl:value-of select="@reason"/>
                <xsl:text>) </xsl:text>
                </xsl:if>
                <div class="extra">
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
                    <hr/>
                  </div> 
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
        <xsl:if test="position() &lt; 2">
          <span class="verbosity">Toggle all verbosity:  </span> 
          <input id="policy-toggle-{generate-id()}" class="toggle toggle-round" type="checkbox"/>
          <label for="policy-toggle-{generate-id()}"></label>
        </xsl:if>
        <table class="mc">
          <xsl:for-each select="mc:check/mc:test">
            <tr>
              <td>
                <input id="policy-arrow-{generate-id()}" class="arrow arrow-mark" type="checkbox"/>
                <label for="policy-arrow-{generate-id()}"></label>
                <xsl:text> </xsl:text>
                <strong>
                  <xsl:value-of select="../@name"/>
                </strong>
                <xsl:text> </xsl:text>
                <xsl:if test="@outcome = 'pass'">
                  <xsl:text>&#x2705;  </xsl:text>
                  <xsl:value-of select="@outcome"/>
                </xsl:if>
                <xsl:if test="@outcome = 'fail'">
                  <xsl:text>&#x274C;  </xsl:text>
                <xsl:value-of select="@outcome"/>
                (Reason: <xsl:value-of select="@reason"/>
                <xsl:text>)</xsl:text>
                </xsl:if>
                <xsl:if test="@outcome = 'N/A'">
                <xsl:value-of select="@outcome"/>
                </xsl:if>
                <div class="extra">
                    <xsl:if test="../mc:context/@field != ''">
                        <strong><xsl:text>Context (field): </xsl:text></strong>
                        <xsl:value-of select="../mc:context/@field"/><br/>
                    </xsl:if>
                    <xsl:if test="../mc:context/@value != ''">
                        <strong><xsl:text>Context (value): </xsl:text></strong>
                        <xsl:value-of select="../mc:context/@value"/><br/>
                    </xsl:if>
                    <xsl:if test="@tracktype != ''">
                      <strong>Track type: </strong> <xsl:value-of select="@tracktype"/><br/>
                    </xsl:if>
                   <xsl:if test="@tracktypeorder != ''">
                      <strong>Track type order: </strong>  <xsl:value-of select="@tracktypeorder"/><br/>
                  </xsl:if>
                  <xsl:if test="@trackid != ''">
                    <strong>Track ID: </strong>  <xsl:value-of select="@trackid"/><br/>
                  </xsl:if>
                  <xsl:if test="@actual != ''">
                    <strong>Actual: </strong>  <xsl:value-of select="@actual"/><br/>
                  </xsl:if>
                  <hr/>
                </div>
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
