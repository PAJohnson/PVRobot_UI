General plan for UI code layout and design

The directory is as follows:
/files
	/index.html
	/styles.css
	/pages
		/page_1
			/page_1.html
			/page_1.js
			/page_1.css
		/page_2
			/page_2.html
			/page_2.js
			/page_2.css
		/page_n
			/page_n.html
			/page_n.js
			/page_n.css

The UI operates as a Single Page Application. This means that the operator percieves the UI as a single screen or view.

In reality, the page is a single page "index.html", with persistent panels on the left and right that contain the control buttons. The center of the page displays the appropriate page - page_1.html, page_2.html, etc. Thus, each different "screen" in the app is a separate document. This makes development easier. In addition, it encapsulates the code for each page.

index.html and styles.css describe the styling and functionality of the control panel buttons.

To add a button or control to the panels, modify index.html. Currently, the "separate pages" are shown in the <iframe>, but this may be changed in the future. 
			
