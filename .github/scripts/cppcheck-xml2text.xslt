<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" encoding="UTF-8"/>
<xsl:template match="/">## CppCheck <xsl:value-of select="//cppcheck/@version"/> Summary
<xsl:if test="count(//error) > 0">
| error | warning | style | performance | portability | information |
| --- | --- | --- | --- | --- | --- |
| <xsl:value-of select="count(//error[@severity='error'])"/> | <xsl:value-of select="count(//error[@severity='warning'])"/> | <xsl:value-of select="count(//error[@severity='style'])"/> | <xsl:value-of select="count(//error[@severity='performance'])"/> | <xsl:value-of select="count(//error[@severity='portability'])"/> | <xsl:value-of select="count(//error[@severity='information'])"/> |

| severity | location | error id | issue |
| --- | --- | --- | --- |
<xsl:for-each select="results//error">| <xsl:value-of select="@severity"/> | <xsl:value-of select="location/@file"/>:<xsl:value-of select="location/@line"/> | <xsl:value-of select="@id"/> | <xsl:value-of select="@msg"/> |
</xsl:for-each>
</xsl:if>
**<xsl:value-of select="count(//error[@severity='error'])"/> error(s) reported**
</xsl:template>
</xsl:stylesheet>
