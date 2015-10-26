<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="https://mediaarea.net/mediaconch" xmlns:ma="https://mediaarea.net/mediaarea" xmlns:mi="https://mediaarea.net/mediainfo" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi ma">
    <xsl:output encoding="UTF-8" method="xml" version="1.0" indent="yes"/>
    <xsl:template match="ma:MediaArea">
        <MediaConch>
            <xsl:attribute name="version">
                <xsl:text>0.1</xsl:text>
            </xsl:attribute>
            <policyChecks>
                <title>This is the policy set title</title>
                <description>This is the policy set description</description>
                <xsl:for-each select="ma:media">
                    <media>
                        <xsl:attribute name="ref">
                            <xsl:value-of select="./@ref"/>
                        </xsl:attribute>
                        <!-- the for-each loop doesn't apply to is_true functions where the xpath is passed as a user-construction -->
                        <!-- the is_true function and example here is for the special case when the free text entry is used, thus only title and xpath are available -->
                        <policy>
                            <xsl:attribute name="title">Is Matroska or QuickTime</xsl:attribute>
                            <context>
                                <xsl:attribute name="value">mi:MediaInfo/mi:track[@type='General'][1]/mi:Format = 'Matroska' or mi:MediaInfo/mi:track[@type='General'][1]/mi:Format = 'AVI'</xsl:attribute>
                            </context>
                            <xsl:call-template name="is_true">
                                <xsl:with-param name="xpath" select="mi:MediaInfo/mi:track[@type='General'][1]/mi:Format = 'Matroska' or mi:MediaInfo/mi:track[@type='General'][1]/mi:Format = 'AVI'"/>
                            </xsl:call-template>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Is Matroska</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format</xsl:attribute>
                                <xsl:attribute name="value">Matroska</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][1]/mi:Format">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][1]/mi:Format">
                                        <xsl:call-template name="is_equal">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">Matroska</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Is FFV1</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format</xsl:attribute>
                                <xsl:attribute name="value">FFV1</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                                        <xsl:call-template name="is_equal">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">FFV1</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Audio is PCM</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format</xsl:attribute>
                                <xsl:attribute name="value">PCM</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:Format">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Audio'][*]/mi:Format">
                                        <xsl:call-template name="is_equal">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">PCM</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Format version is Version 3.1</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format_Version</xsl:attribute>
                                <xsl:attribute name="value">Version 3.1</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format_Version">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format_Version">
                                        <xsl:call-template name="is_equal">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">3.1</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">FileSize is greater than 0</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">FileSize</xsl:attribute>
                                <xsl:attribute name="value">0</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='General'][1]/mi:FileSize">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='General'][1]/mi:FileSize">
                                        <xsl:call-template name="is_greater_than">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">0</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Video track Format exists</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                                        <xsl:call-template name="exists">
                                            <xsl:with-param name="xpath" select="."/>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Video track Formatzzzzz does not exist</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Formatzzzzz</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Formatzzzzz">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Formatzzzzz">
                                        <xsl:call-template name="does_not_exist">
                                            <xsl:with-param name="xpath" select="."/>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Format contains 1</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format</xsl:attribute>
                                <xsl:attribute name="value">1</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format">
                                        <xsl:call-template name="contains_string">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">1</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                        <policy>
                            <xsl:attribute name="title">Format version does not contain 1</xsl:attribute>
                            <context>
                                <xsl:attribute name="field">Format_Version</xsl:attribute>
                                <xsl:attribute name="value">1</xsl:attribute>
                            </context>
                            <xsl:choose>
                                <xsl:when test="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format_Version">
                                    <xsl:for-each select="mi:MediaInfo/mi:track[@type='Video'][1]/mi:Format_Version">
                                        <xsl:call-template name="contains_string">
                                            <xsl:with-param name="xpath" select="."/>
                                            <xsl:with-param name="value">1</xsl:with-param>
                                        </xsl:call-template>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <test outcome="invalid"/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </policy>
                    </media>
                </xsl:for-each>
            </policyChecks>
        </MediaConch>
    </xsl:template>
    <xsl:template name="is_true">
        <xsl:param name="xpath"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:choose>
                <xsl:when test="$xpath">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is not true</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="is_equal">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$xpath = $value">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is not equal</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="is_not_equal">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$xpath != $value">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is equal</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="is_greater_than">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$xpath &gt; $value">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is less than or equal</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="is_less_than">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$xpath &lt; $value">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is greater than or equal</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="is_greater_or_equal_than">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$xpath &gt;= $value">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is less than</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="is_less_or_equal_than">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$xpath &lt;= $value">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">is greater than</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="exists">
        <xsl:param name="xpath"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="string-length($xpath) != 0">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">does not exist</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="does_not_exist">
        <xsl:param name="xpath"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="string-length($xpath) = '0'">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">exists</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
    <xsl:template name="contains_string">
        <xsl:param name="xpath"/>
        <xsl:param name="value"/>
        <xsl:element name="test">
            <xsl:if test="../@type">
                <xsl:attribute name="tracktype">
                    <xsl:value-of select="../@type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:if test="../@streamid">
                <xsl:attribute name="streamid">
                    <xsl:value-of select="../@streamid"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:attribute name="actual">
                <xsl:value-of select="$xpath"/>
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="contains($xpath, $value)">
                    <xsl:attribute name="outcome">pass</xsl:attribute>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:attribute name="outcome">fail</xsl:attribute>
                    <xsl:attribute name="reason">does not contain</xsl:attribute>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:element>
    </xsl:template>
</xsl:stylesheet>
