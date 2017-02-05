<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:aliasxsl="my:namespace" version="1.0" exclude-result-prefixes="aliasxsl">
  <xsl:param name="compare" as="xsl:string"/>
  <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
  <xsl:template match="/">
    <aliasxsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:mc="https://mediaarea.net/mediaconch" xmlns:ma="https://mediaarea.net/mediaarea" xmlns:mmt="https://mediaarea.net/micromediatrace" xmlns:mi="https://mediaarea.net/mediainfo" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:exsl="http://exslt.org/common" version="1.0" extension-element-prefixes="exsl xsi ma mc">
      <aliasxsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
      <aliasxsl:template match="ma:MediaArea">
        <MediaConch>
          <aliasxsl:attribute name="version">
            <aliasxsl:text>0.3</aliasxsl:text>
          </aliasxsl:attribute>
          <aliasxsl:for-each select="ma:media">
            <media>
              <aliasxsl:attribute name="ref">
                <aliasxsl:value-of select="./@ref"/>
              </aliasxsl:attribute>
              <xsl:if test="string-length($compare)>0">
                <aliasxsl:attribute name="compare">
                  <xsl:value-of select="document($compare)//ma:media[1]/@ref"/>
                </aliasxsl:attribute>
              </xsl:if>
              <xsl:for-each select="policy">
                <xsl:call-template name="policycheck">
                  <xsl:with-param name="policy" select="."/>
                </xsl:call-template>
              </xsl:for-each>
              <xsl:for-each select="rule">
                <xsl:if test="@tracktype or @scope">
                  <xsl:call-template name="rulecheck">
                    <xsl:with-param name="rule" select="."/>
                  </xsl:call-template>
                </xsl:if>
              </xsl:for-each>
            </media>
          </aliasxsl:for-each>
        </MediaConch>
      </aliasxsl:template>
      <aliasxsl:template name="rule">
        <aliasxsl:param name="name"/>
        <aliasxsl:param name="scope"/>
        <aliasxsl:param name="value"/>
        <aliasxsl:param name="tracktype"/>
        <aliasxsl:param name="occurrence"/>
        <aliasxsl:param name="operator"/>
        <aliasxsl:param name="xpath"/>
        <aliasxsl:param name="outcome"/>
        <aliasxsl:param name="actual"/>
        <aliasxsl:param name="compared_to"/>
        <rule>
          <aliasxsl:if test="../@type">
            <aliasxsl:attribute name="tracktype">
              <aliasxsl:value-of select="../@type"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="../@typeorder">
            <aliasxsl:attribute name="tracktypeorder">
              <aliasxsl:value-of select="../@typeorder"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="../mi:ID">
            <aliasxsl:attribute name="trackid">
              <aliasxsl:value-of select="../mi:ID"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$name">
            <aliasxsl:attribute name="name">
              <aliasxsl:value-of select="$name"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$scope">
            <aliasxsl:attribute name="scope">
              <aliasxsl:value-of select="$scope"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$value">
            <aliasxsl:attribute name="value">
              <aliasxsl:value-of select="$value"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$tracktype">
            <aliasxsl:attribute name="tracktype">
              <aliasxsl:value-of select="$tracktype"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$occurrence">
            <aliasxsl:attribute name="occurrence">
              <aliasxsl:value-of select="$occurrence"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$operator">
            <aliasxsl:attribute name="operator">
              <aliasxsl:value-of select="$operator"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:attribute name="xpath">
            <aliasxsl:value-of select="$xpath"/>
          </aliasxsl:attribute>
          <aliasxsl:if test="$outcome='fail' or string-length($compared_to)>0 or $operator='starts with' or $operator='must not start with'">
            <aliasxsl:attribute name="actual">
              <aliasxsl:value-of select="$actual"/>
            </aliasxsl:attribute>
          </aliasxsl:if>
          <aliasxsl:if test="$outcome='fail'">
            <aliasxsl:if test="string-length($compared_to)>0">
              <aliasxsl:attribute name="compared_to">
                <aliasxsl:value-of select="$compared_to"/>
              </aliasxsl:attribute>
            </aliasxsl:if>
          </aliasxsl:if>
          <aliasxsl:attribute name="outcome">
            <aliasxsl:value-of select="$outcome"/>
          </aliasxsl:attribute>
        </rule>
      </aliasxsl:template>
      <aliasxsl:template name="check">
        <aliasxsl:param name="name"/>
        <aliasxsl:param name="description"/>
        <aliasxsl:param name="type"/>
        <aliasxsl:param name="context"/>
        <aliasxsl:param name="value"/>
        <aliasxsl:param name="rule"/>
        <aliasxsl:param name="ruleresults"/>
        <aliasxsl:param name="morepolicies"/>
        <policy>
          <aliasxsl:attribute name="name">
            <aliasxsl:value-of select="$name"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="type">
            <aliasxsl:value-of select="$type"/>
          </aliasxsl:attribute>
          <aliasxsl:variable name="rules_run">
            <aliasxsl:value-of select="count(exsl:node-set($ruleresults)/mc:rule) + count(exsl:node-set($morepolicies)/mc:policy)"/>
          </aliasxsl:variable>
          <aliasxsl:variable name="fail_count">
            <aliasxsl:value-of select="count(exsl:node-set($ruleresults)/mc:rule[@outcome='fail']) + count(exsl:node-set($morepolicies)/mc:policy[@outcome='fail'])"/>
          </aliasxsl:variable>
          <aliasxsl:variable name="pass_count">
            <aliasxsl:value-of select="count(exsl:node-set($ruleresults)/mc:rule[@outcome='pass']) + count(exsl:node-set($morepolicies)/mc:policy[@outcome='pass'])"/>
          </aliasxsl:variable>
          <aliasxsl:attribute name="rules_run">
            <aliasxsl:value-of select="$rules_run"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="fail_count">
            <aliasxsl:value-of select="$fail_count"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="pass_count">
            <aliasxsl:value-of select="$pass_count"/>
          </aliasxsl:attribute>
          <aliasxsl:attribute name="outcome">
            <aliasxsl:choose>
              <aliasxsl:when test="$type = 'or' and $pass_count &gt;= 1">pass</aliasxsl:when>
              <aliasxsl:when test="$type = 'and' and $fail_count = '0'">pass</aliasxsl:when>
              <aliasxsl:otherwise>fail</aliasxsl:otherwise>
            </aliasxsl:choose>
          </aliasxsl:attribute>
          <aliasxsl:if test="$description">
            <description>
              <aliasxsl:value-of select="$description"/>
            </description>
          </aliasxsl:if>
          <aliasxsl:copy-of select="$context"/>
          <aliasxsl:copy-of select="$ruleresults"/>
          <aliasxsl:copy-of select="$morepolicies"/>
          <!-- disable this output for now
          <aliasxsl:choose>
            <aliasxsl:when test="count(exsl:node-set($ruleresults)/mc:rule)+count(exsl:node-set($morepolicies)/mc:policy)='0'">
              <rule outcome="N/A"/>
            </aliasxsl:when>
          </aliasxsl:choose>
          -->
        </policy>
      </aliasxsl:template>
    </aliasxsl:stylesheet>
  </xsl:template>
  <xsl:template name="policycheck">
    <xsl:for-each select=".">
      <aliasxsl:call-template name="check">
        <aliasxsl:with-param name="name">
          <xsl:value-of select="@name"/>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="description">
          <xsl:value-of select="description"/>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="type">
          <xsl:value-of select="@type"/>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="ruleresults">
          <xsl:for-each select="rule">
            <xsl:if test="@tracktype or @scope">
              <xsl:call-template name="rulecheck">
                <xsl:with-param name="rule" select="."/>
              </xsl:call-template>
            </xsl:if>
          </xsl:for-each>
        </aliasxsl:with-param>
        <aliasxsl:with-param name="morepolicies">
          <xsl:for-each select="policy">
            <xsl:call-template name="policycheck">
              <xsl:with-param name="policy" select="."/>
            </xsl:call-template>
          </xsl:for-each>
        </aliasxsl:with-param>
      </aliasxsl:call-template>
    </xsl:for-each>
  </xsl:template>
  <xsl:template name="rulecheck">
    <xsl:param name="rule"/>
    <xsl:variable name="equationbase">
      <xsl:choose>
        <xsl:when test="@scope='mmt'">
          <xsl:text>mmt:MicroMediaTrace</xsl:text>
          <xsl:call-template name="tokenize">
            <xsl:with-param name="list" select="@value"/>
            <xsl:with-param name="delimiter" select="'/'"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>mi:MediaInfo/mi:track[@type='</xsl:text>
          <xsl:value-of select="@tracktype"/>
          <xsl:text>']</xsl:text>
          <xsl:if test="@occurrence">
            <xsl:text>[</xsl:text>
            <xsl:value-of select="@occurrence"/>
            <xsl:text>]</xsl:text>
          </xsl:if>
          <xsl:call-template name="tokenize">
            <xsl:with-param name="list" select="@value"/>
            <xsl:with-param name="delimiter" select="'/'"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="equationfull">
      <xsl:choose>
        <xsl:when test="not(@operator) or @operator='exists'"></xsl:when>
        <xsl:when test="@operator='must not exist'">not(</xsl:when>
        <xsl:when test="@operator='starts with'">starts-with(</xsl:when>
        <xsl:when test="@operator='must not start with'">not(starts-with(</xsl:when>
      </xsl:choose>
      <xsl:value-of select="$equationbase"/>
      <xsl:choose>
        <xsl:when test="not(@operator) or @operator='exists'"></xsl:when>
        <xsl:when test="@operator='must not exist'">)</xsl:when>
        <xsl:when test="@operator='starts with' or @operator='must not start with'">
          <xsl:text>,'</xsl:text>
          <xsl:value-of select="."/>
          <xsl:text>')</xsl:text>
          <xsl:if test="@operator='must not start with'">)</xsl:if>
        </xsl:when>
        <xsl:when test="@operator">
          <xsl:value-of select="@operator"/>
          <xsl:choose>
            <xsl:when test=".='compare' and string-length($compare)>0">
              <xsl:text>document('</xsl:text>
              <xsl:value-of select="$compare"/>
              <xsl:text>')//</xsl:text>
              <xsl:value-of select="$equationbase"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>'</xsl:text>
              <xsl:value-of select="."/>
              <xsl:text>'</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>mi:MediaInfo/mi:track[@type='</xsl:text>
          <xsl:value-of select="@tracktype"/>
          <xsl:text>']</xsl:text>
          <xsl:if test="@occurrence">
            <xsl:text>[</xsl:text>
            <xsl:value-of select="@occurrence"/>
            <xsl:text>]</xsl:text>
          </xsl:if>
          <xsl:call-template name="tokenize">
            <xsl:with-param name="list" select="@value"/>
            <xsl:with-param name="delimiter" select="'/'"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="compare_xpath">
      <xsl:if test=".='compare' and string-length($compare)>0">
        <xsl:text>document('</xsl:text>
        <xsl:value-of select="$compare"/>
        <xsl:text>')//</xsl:text>
        <xsl:choose>
          <xsl:when test="@scope='mmt'">
            <xsl:text>mmt:MicroMediaTrace</xsl:text>
            <xsl:call-template name="tokenize">
              <xsl:with-param name="list" select="@value"/>
              <xsl:with-param name="delimiter" select="'/'"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>mi:MediaInfo/mi:track[@type='</xsl:text>
            <xsl:value-of select="@tracktype"/>
            <xsl:text>'][</xsl:text>
            <xsl:value-of select="@occurrence"/>
            <xsl:text>]</xsl:text>
            <xsl:call-template name="tokenize">
              <xsl:with-param name="list" select="@value"/>
              <xsl:with-param name="delimiter" select="'/'"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </xsl:variable>
    <aliasxsl:call-template name="rule">
      <aliasxsl:with-param name="name">
        <xsl:value-of select="@name"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="scope">
        <xsl:value-of select="@scope"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="tracktype">
        <xsl:value-of select="@tracktype"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="occurrence">
        <xsl:value-of select="@occurrence"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="operator">
        <xsl:value-of select="@operator"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="xpath">
        <xsl:value-of select="$equationfull"/>
      </aliasxsl:with-param>
      <aliasxsl:with-param name="actual">
        <xsl:attribute name="select">
          <xsl:value-of select="$equationbase"/>
        </xsl:attribute>
      </aliasxsl:with-param>
      <xsl:if test=".='compare' and string-length($compare)>0">
        <aliasxsl:with-param name="compared_to">
          <xsl:attribute name="select">
            <xsl:value-of select="$compare_xpath"/>
          </xsl:attribute>
        </aliasxsl:with-param>
      </xsl:if>
      <aliasxsl:with-param name="outcome">
        <aliasxsl:choose>
          <aliasxsl:when>
            <xsl:attribute name="test">
              <xsl:value-of select="$equationfull"/>
            </xsl:attribute>
            <xsl:text>pass</xsl:text>
          </aliasxsl:when>
          <aliasxsl:otherwise>fail</aliasxsl:otherwise>
        </aliasxsl:choose>
      </aliasxsl:with-param>
    </aliasxsl:call-template>
  </xsl:template>
  <xsl:template name="tokenize">
    <xsl:param name="list"/>
    <xsl:param name="delimiter"/>
    <xsl:variable name="removeprefix">
      <xsl:choose>
        <xsl:when test="substring($list, 1 , 7)='offset:'">
          <xsl:value-of select="substring-after($list,'offset:')"/>
        </xsl:when>
        <xsl:when test="substring($list, 1 , 5)='size:'">
          <xsl:value-of select="substring-after($list,'size:')"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$list"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="newlist">
      <xsl:choose>
        <xsl:when test="contains($removeprefix, $delimiter)">
          <xsl:value-of select="normalize-space($removeprefix)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat(normalize-space($removeprefix), $delimiter)"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="first" select="substring-before($newlist, $delimiter)"/>
    <xsl:variable name="remaining" select="substring-after($newlist, $delimiter)"/>
    <xsl:variable name="count" select="position()"/>
    <xsl:choose>
      <xsl:when test="$remaining">
        <xsl:choose>
          <xsl:when test="substring($first,1,2)='m:'">
            <xsl:choose>
              <xsl:when test="@scope='mmt'">
                <xsl:text>/mmt:b[mmt:b[@n='Header']/mmt:d[@n='Name']='</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>/mi:</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:value-of select="substring($first,3)"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="@scope='mmt'">
                <xsl:text>/mmt:b[@n='</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>/mi:</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:value-of select="$first"/>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="@scope='mmt'">
            <xsl:text>']</xsl:text>
          </xsl:when>
        </xsl:choose>
        <xsl:call-template name="tokenize">
          <xsl:with-param name="list" select="$remaining"/>
          <xsl:with-param name="delimiter">
            <xsl:value-of select="$delimiter"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="$first='Data'">
            <xsl:choose>
              <xsl:when test="@scope='mmt'">
                <xsl:text>/mmt:d</xsl:text>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="substring($first,1,2)='m:'">
                <xsl:choose>
                  <xsl:when test="@scope='mmt'">
                    <xsl:text>/mmt:b[mmt:b[@n='Header']/mmt:d[@n='Name']='</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>/mi:</xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
                <xsl:value-of select="substring($first,3)"/>
                <xsl:choose>
                  <xsl:when test="@scope='mmt'">
                    <xsl:text>']</xsl:text>
                  </xsl:when>
                </xsl:choose>
              </xsl:when>
              <xsl:when test="substring($list, string-length($list)) = '/'">
                <xsl:choose>
                  <xsl:when test="@scope='mmt'">
                    <xsl:text>/mmt:b[@n='</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>/mi:</xsl:otherwise>
                </xsl:choose>
                <xsl:value-of select="$first"/>
                <xsl:choose>
                  <xsl:when test="@scope='mmt'">
                    <xsl:text>']</xsl:text>
                  </xsl:when>
                </xsl:choose>
              </xsl:when>
              <xsl:otherwise>
                <xsl:choose>
                  <xsl:when test="@scope='mmt'">
                    <xsl:text>/mmt:d[@n='</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>/mi:</xsl:otherwise>
                </xsl:choose>
                <xsl:value-of select="$first"/>
                <xsl:choose>
                  <xsl:when test="@scope='mmt'">
                    <xsl:text>']</xsl:text>
                  </xsl:when>
                </xsl:choose>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="substring-before($list, ':') = 'offset'">
        <xsl:choose>
          <xsl:when test="@scope='mmt'">
            <xsl:text>/@o</xsl:text>
          </xsl:when>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="substring-before($list, ':') = 'size'">
        <xsl:choose>
          <xsl:when test="@scope='mmt'">
            <xsl:text>/@s</xsl:text>
          </xsl:when>
        </xsl:choose>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
