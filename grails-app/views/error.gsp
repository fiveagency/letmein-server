<!doctype html>
<html>
    <head>
        <meta name="layout" content="main">
        <link rel="stylesheet" href="${resource(dir: 'css', file: 'errors.css')}" type="text/css">
    </head>
    <body>
        <g:if test="${flash.error}">
        <ul class="errors"><li>${flash.error}</li></ul>
        </g:if>
        <g:renderException exception="${exception}" />
    </body>
</html>