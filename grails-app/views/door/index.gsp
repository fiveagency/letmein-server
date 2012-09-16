
<%@ page import="eu.fiveminutes.lmi.Door" %>
<!doctype html>
<html>
    <head>
        <meta name="layout" content="main">
    </head>
    <body>
        <a href="#show-door" class="skip" tabindex="-1"><g:message code="default.link.skip.label" default="Skip to content&hellip;"/></a>
        <div id="show-door" class="content scaffold-show" role="main">
            <g:if test="${flash.message}">
            <div class="message" role="status">${flash.message}</div>
            </g:if>
            <g:form>
                <g:actionSubmitImage  class="redButton" src="${resource(dir: 'images', file: 'red_button.png')}" action="openAuth" 
                    value="${message(code: 'default.button.open.label', default: 'Open door')}" />
            </g:form>
        </div>
    </body>
</html>
