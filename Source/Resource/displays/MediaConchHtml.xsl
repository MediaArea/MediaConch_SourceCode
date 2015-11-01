<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="UTF-8" method="html" version="1.0" indent="yes"/>
  <xsl:template match="/mc:MediaConch">
    <xsl:for-each select="mc:media">
      <xsl:text> &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A; </xsl:text>
      <b>
        <xsl:value-of select="@ref"/>
      </b>
      <xsl:for-each select="mc:implementationChecks">
        <h1>
          <xsl:value-of select="mc:name"/>
        </h1>
        <p>
          <xsl:value-of select="mc:description"/>
        </p>
        <table style="border: none;border-collapse:separate;border-spacing:0px 1em">
            <xsl:for-each select="mc:check">
              <tr>
                <td style="border:1px solid black">
                  <p>
                    <xsl:value-of select="@icid"/>
                  </p>
                  <xsl:if test="mc:context/@name != ''">
                    <p>
                      <xsl:text>Context (name): </xsl:text>
                      <xsl:value-of select="mc:context/@name"/>
                    </p>
                  </xsl:if>
                  <table border="1">
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
        <xsl:text> &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A;  &#x1F41A; </xsl:text>
        <h1>
          <xsl:value-of select="mc:name"/>
        </h1>
        <p>
          <xsl:value-of select="mc:description"/>
        </p>
        <table style="border: none;border-collapse:separate;border-spacing:0px 1em">
          <xsl:for-each select="mc:check">
            <tr>
              <td style="border:1px solid black">
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
                <table border="1">
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
  </xsl:template>
</xsl:stylesheet>
