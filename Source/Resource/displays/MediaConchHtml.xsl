<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:x="http://www.w3.org/1999/xhtml" version="1.0" extension-element-prefixes="xsi" exclude-result-prefixes="x" >
  <xsl:output encoding="UTF-8" method="html" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
    <xsl:for-each select="mc:media">
      <div class="mc_header">
        <div>
        <xsl:text>&#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A; &#x1F41A;</xsl:text>
        </div>
        <h1>MediaConch Report</h1>
        <div>
        <xsl:text>&#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A; &#x1F41A;</xsl:text>
        </div>
        <b>
          <p>&#x1F41A; File path:  <xsl:value-of select="@ref"/></p>
        </b>
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
        <table id="mc_implementation">
            <xsl:for-each select="mc:check">
              <tr>
                <td>
                  <p>
                    <xsl:value-of select="@icid"/>
                  </p>
                  <xsl:if test="mc:context/@name != ''">
                    <p>
                      <xsl:text>Context (name): </xsl:text>
                      <xsl:value-of select="mc:context/@name"/>
                    </p>
                  </xsl:if>
                  <table>
                    <tr>
                      <th>value</th>
                      <th>outcome</th>
                      <th>reason</th>
                    </tr>
                    <xsl:for-each select="mc:test">
                      <tr>
                        <td>
                          <xsl:for-each select="mc:value">
                            <xsl:if test="@name != ''">
                              <xsl:value-of select="@name"/>
                              <xsl:text>=</xsl:text>
                            </xsl:if>
                            <xsl:value-of select="."/>
                            <br/>
                          </xsl:for-each>
                        </td>
                        <td>
                          <xsl:if test="@outcome = 'pass'">
                            <xsl:text>&#x2705;  </xsl:text>
                          </xsl:if>
                          <xsl:if test="@outcome = 'fail'">
                            <xsl:text>&#x274C;  </xsl:text>
                          </xsl:if>
                          <xsl:value-of select="@outcome"/>
                        </td>
                        <td>
                          <xsl:value-of select="@reason"/>
                        </td>
                      </tr>
                    </xsl:for-each>
                  </table>
                </td>
              </tr>
            </xsl:for-each>
          </table>
      </xsl:for-each>
      <xsl:for-each select="mc:policyChecks">
        <div class="mc_header">
          <h2>
            <xsl:value-of select="mc:name"/>
          </h2>
          <p>
            <xsl:value-of select="mc:description"/>
          </p>
        </div>
        <table id="mc_policy">
          <xsl:for-each select="mc:check">
            <tr>
              <td>
                <p>
                  <xsl:value-of select="@name"/>
                </p>
                <xsl:if test="mc:context/@field != ''">
                  <p>
                    <xsl:text>Context (field): </xsl:text>
                    <xsl:value-of select="mc:context/@field"/>
                  </p>
                </xsl:if>
                <xsl:if test="mc:context/@value != ''">
                  <p>
                    <xsl:text>Context (value): </xsl:text>
                    <xsl:value-of select="mc:context/@value"/>
                  </p>
                </xsl:if>
                <table>
                  <tr>
                    <th>tracktype</th>
                    <th>tracktypeorder</th>
                    <th>trackid</th>
                    <th>actual</th>
                    <th>outcome</th>
                    <th>reason</th>
                  </tr>
                  <xsl:for-each select="mc:test">
                    <tr>
                      <td>
                        <xsl:value-of select="@tracktype"/>
                      </td>
                      <td>
                        <xsl:value-of select="@tracktypeorder"/>
                      </td>
                      <td>
                        <xsl:value-of select="@trackid"/>
                      </td>
                      <td>
                        <xsl:value-of select="@actual"/>
                      </td>
                      <td>
                        <xsl:if test="@outcome = 'pass'">
                          <xsl:text>&#x2705;  </xsl:text>
                        </xsl:if>
                        <xsl:if test="@outcome = 'fail'">
                          <xsl:text>&#x274C;  </xsl:text>
                        </xsl:if>
                        <xsl:value-of select="@outcome"/>
                      </td>
                      <td>
                        <xsl:value-of select="@reason"/>
                      </td>
                    </tr>
                  </xsl:for-each>
                </table>
              </td>
            </tr>
          </xsl:for-each>
        </table>
      </xsl:for-each>
    </xsl:for-each>
    <style>
.mc_header {
  width   : 600px;
  border: 1px black solid;
  background-color: #64A8DD;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

.mc_header h1, h2, div {
  text-align: center;
}

#mc_implementation {
  width   : 600px;
  background-color: #F2F5F8;
  border: 1px white solid;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_implementation h1  {
  border: 1px white solid;
  background-color: #64A8DD;
}

#mc_implementation table {
  border: 1px black solid;
  width   : 600px;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_implementation th {
  border-bottom: 1px black solid;
  text-align:left;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_implementation p {
  background-color: #64A8DD;
  font-size: 18px;
  font-weight: 700;
  padding: 10px;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_policy {
  width   : 600px;
  background-color: #F2F5F8;
  border: 1px white solid;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_policy h1  {
  border: 1px white solid;
  background-color: #64A8DD;
}

#mc_policy table {
  border: 1px black solid;
  width   : 600px;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_policy th {
  border-bottom: 1px black solid;
  text-align:left;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}

#mc_policy p {
  background-color: #64A8DD;
  font-size: 18px;
  font-weight: 700;
  padding: 10px;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
}
    </style>
  </xsl:template>
</xsl:stylesheet>