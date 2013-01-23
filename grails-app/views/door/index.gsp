<%@ page import="eu.fiveminutes.lmi.Door" %>
<!doctype html>
<html>
    <head>
        <meta name="layout" content="main">
        <script type="text/javascript">
        function showOpenButtonPressed() {
            $('#doorButton').hide();
            $('#doorButtonPressed').show();
        }
        function hideOpenButtonPressed() {
            $('#doorButtonPressed').hide();
            $('#doorButton').show();
        }
        </script>
    </head>
    <body>
        <a href="#show-door" class="skip" tabindex="-1"><g:message code="default.link.skip.label" default="Skip to content&hellip;"/></a>
        <div id="show-door" class="content scaffold-show" role="main">
            <g:if test="${flash.message}">
            <div class="message" role="status">${flash.message}</div>
            </g:if>
            <!-- 
            <img class="cameraStream" src="http://${grailsApplication.config.camera.ip}/img/video.mjpeg">
             -->
            <g:remoteLink action="openAuth" onLoading="showOpenButtonPressed()" onComplete="hideOpenButtonPressed()" >
                <img id="doorButton" class="doorButton" src="<g:resource dir="images" file="red_button.png"/> "/>
                <img id="doorButtonPressed" class="doorButton" style="display: none;" src="<g:resource dir="images" file="kapow.png"/> "/>
            </g:remoteLink>
            
        </div>
    </body>
</html>
