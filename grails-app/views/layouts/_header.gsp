<div id="grailsLogo" role="banner"><img src="${resource(dir: 'images', file: 'grails_logo.png')}" alt="Let Me In">
	<span class="header-main">Let Me In</span>  
	<span id="loginHeader"><g:loginControl /></span>  
</div>
<div class="nav" role="navigation">
	<ul>
		<sec:ifLoggedIn>
			<li><g:link class="home" controller="door"><g:message code="default.door.label" default="Door"/></g:link></li>
		</sec:ifLoggedIn>
	</ul>
</div>