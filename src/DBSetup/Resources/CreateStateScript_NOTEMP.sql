
-- Copyright ADC Cure, Amsterdam (c) 2004-2014
-- updated 2014, 2015 June 2015. Option to create no TempDB
-- please note that this script or any parts of ISP Session is not 'open source' or freeware. 
-- We might license parts of ISP Session if you indicate interest for it. 
-- This script creates a SQL Server 2005 database for ISP Session 6.0 + 6.5

-- ISP Session 6.5/6.0; 

CREATE PROC  [dbo].[CreateTempTables]
AS
BEGIN

  	EXEC dbo.FixSecurityForTempDb
	
	
END

GO

	CREATE TABLE [dbo].[tblSessions_ISP]
		(
		[App_Key] binary (16) NOT NULL ,
		[GUID] binary (16) NOT NULL ,
		[Started] datetime NOT NULL ,
		[LastUpdated] datetime NOT NULL ,
	
		[Expires] int NOT NULL ,
		[ReEntrance] bit NOT NULL ,
		[Liquid] bit NOT NULL ,
		[State] varbinary (max) NULL,
		[zLen] int NOT NULL,
		[timest] timestamp NOT NULL
		)
		GO
	CREATE TABLE [dbo].[tblIspSession_Stats]
	(	statid int NOT NULL,
		APP_KEY binary(16) NOT NULL,
		SessionCount int NOT NULL CONSTRAINT DF_tblIspSession_Stats_SessionCount  DEFAULT ((0)),
		AvgStateLength int NOT NULL CONSTRAINT DF_tblIspSession_Stats_statvalue  DEFAULT ((0)),
		MinStateLength int NOT NULL CONSTRAINT DF_tblIspSession_Stats_MinShortStateLength  DEFAULT ((0)),
		MaxStateLength int NOT NULL CONSTRAINT DF_tblIspSession_Stats_MaxShortStateLength  DEFAULT ((0)),
		AvgCompressionRate real NOT NULL CONSTRAINT DF_tblIspSession_Stats_AvgCompressionRate  DEFAULT ((0)),
		AvgSessionDuration int NOT NULL CONSTRAINT DF_tblIspSession_Stats_AvgSessionDuration  DEFAULT ((0)),
		CountExpired int NOT NULL CONSTRAINT DF_tblIspSession_Stats_CountExpired  DEFAULT ((0)))
		Go
	ALTER TABLE [dbo].[tblIspSession_Stats] WITH NOCHECK  ADD 
		CONSTRAINT  PK_tblIspSession_Stats PRIMARY KEY CLUSTERED 
		(statid , APP_KEY ) 

	ALTER TABLE dbo.tblSessions_ISP ADD 
		CONSTRAINT DF_tblSessions_ISP_zLen DEFAULT (0) FOR zLen,
		CONSTRAINT PK_tblSessions_ISP PRIMARY KEY  NONCLUSTERED --DO NOT MODIFY TO CLUSTERED
		(
			GUID
		)  WITH FILLFACTOR=75 ON [PRIMARY] 

		GO
	CREATE TABLE [dbo].[tblPageRequestTime](
		[GUID] [binary](16) NOT NULL,
		[PageRequestTime] [smallint] NOT NULL,
		[ErrorMessage] [varchar](255) NULL
	) ON [PRIMARY]

	GO
	CREATE NONCLUSTERED INDEX [IX_tblPageRequestTime] ON [dbo].[tblPageRequestTime]
	(
		[GUID] ASC
	) WITH (FILLFACTOR = 100)
GO

CREATE PROC  [dbo].pSessionFetch
(
	@App_Key binary(16),
	@GUID binary(16)
	
)
AS
	 
SET NOCOUNT ON;
	 SELECT	[GUID], 
			[LastUpdated],  
			[Expires], 
			[ReEntrance], 
			[Liquid], 
			[zLen],
			[timest],
			[State]
	 FROM [dbo].tblSessions_ISP 
	WHERE [GUID] = @GUID 
		AND [App_Key] = @App_Key;
GO

GO
CREATE PROC  [dbo].FetchAllTimeOut
AS
	SET NOCOUNT ON;
		SELECT [APP_KEY], [GUID]
	FROM [dbo].TBLSESSIONS_ISP (NOLOCK) 
	WHERE DATEDIFF(minute, LastUpdated, GetDate()) > Expires
GO


CREATE PROC  [dbo].pSessionInsert
	@App_Key binary(16), 
	@GUID binary(16), 
	@Expires int, 
	@ReEntrance bit, 
	@liquid bit,
	@InitialState varbinary(max) = null,
	@zLen int = 0
AS
SET NOCOUNT ON;
DECLARE @defdate datetime;
SET @defdate = GetDate();
	
	IF NOT EXISTS(SELECT GUID FROM [dbo].tblSessions_ISP WHERE [GUID]=@GUID AND [App_Key] = @App_Key)
	INSERT INTO dbo.tblSessions_ISP
	           ([App_Key], [GUID], [Started], [LastUpdated], [Expires], [ReEntrance], [Liquid], [zLen],[State])
		VALUES     (@App_Key, @GUID, @defdate, @defdate, @Expires, @ReEntrance, @liquid, @zLen, @InitialState);
GO

	
	CREATE PROC  [dbo].[pSessionSave]
	(
		@App_Key binary(16),
		@GUID binary(16),
		@LastUpdated datetime,
		@Expires int,
		@ReEntrance bit,
		@Liquid bit,
		@prevTimeStamp timestamp,
		@GUIDNew binary(16) = NULL,
		@StateEnc varbinary(max),
		@zLen int = 0,
		@pageRequestTime int = 0
	)
	AS
		
		SET NOCOUNT ON;
		IF @StateEnc IS NULL BEGIN
			If @zlen = 0 OR @zLen IS NULL BEGIN
				IF @GUIDNew IS NULL  
					UPDATE dbo.tblSessions_ISP
						SET LastUpdated = @LastUpdated,
						Expires = @Expires,
						ReEntrance = @ReEntrance,
						Liquid = @Liquid
					WHERE [GUID] = @GUID
					AND App_Key = @App_Key
					AND timest = @prevTimeStamp; 
					
				ELSE 
				UPDATE dbo.tblSessions_ISP
						SET LastUpdated = @LastUpdated,
						Expires = @Expires,
						ReEntrance = @ReEntrance,
						Liquid = @Liquid,
						[GUID] = @GUIDNew
					WHERE [GUID] = @GUID
					AND App_Key = @App_Key
					AND timest = @prevTimeStamp; 
			END
			ELSE BEGIN -- zlen is given
				IF @GUIDNew IS NULL  
					UPDATE dbo.tblSessions_ISP
						SET LastUpdated = @LastUpdated,
						Expires = @Expires,
						ReEntrance = @ReEntrance,
						Liquid = @Liquid, 
						zLen = @zLen
					WHERE [GUID] = @GUID
					AND App_Key = @App_Key
					AND timest = @prevTimeStamp; 
					
				ELSE 
				UPDATE dbo.tblSessions_ISP
						SET LastUpdated = @LastUpdated,
						Expires = @Expires,
						ReEntrance = @ReEntrance,
						Liquid = @Liquid,
						[GUID] = @GUIDNew,
						zLen = @zLen
					WHERE [GUID] = @GUID
					AND App_Key = @App_Key
					AND timest = @prevTimeStamp; 
			END
			
		END	
		ELSE  BEGIN
			
			IF @GUIDNew IS NULL BEGIN
				UPDATE dbo.tblSessions_ISP
					SET LastUpdated = @LastUpdated,
					Expires = @Expires,
					ReEntrance = @ReEntrance,
					Liquid = @Liquid,
					[State] = @StateEnc,
					zLen = @zLen
				WHERE [GUID] = @GUID
				AND App_Key = @App_Key
					AND timest = @prevTimeStamp; 
				
			END 
			ELSE 
				UPDATE dbo.tblSessions_ISP
					SET LastUpdated = @LastUpdated,
					Expires = @Expires,
					ReEntrance = @ReEntrance,
					Liquid = @Liquid,
					[GUID] = @GUIDNew,
					[State] = @StateEnc,
					zLen = @zLen
				WHERE [GUID] = @GUID
				AND App_Key = @App_Key
					AND timest = @prevTimeStamp; 
		END
		if (NOT @pageRequestTime IS NULL) BEGIN
			INSERT dbo.tblPageRequestTime ([GUID], pageRequestTime) 
			VALUES(@GUID, @pageRequestTime)
		END
	
	
GO

CREATE PROC  [dbo].[pSessionGetStats]
	@App_Key binary(16) ,
	@GUID binary(16) = NULL
AS
BEGIN
	SET NOCOUNT ON;
	IF (NOT @GUID IS NULL) BEGIN
		DECLARE @AverageRequestTime int, @totalRequestTime int, 
			@TotalRequests int, @MaxRequestTime int;
	
		SELECT @AverageRequestTime = AVG(P.PageRequestTime) , 
			@totalRequestTime = SUM(P.PageRequestTime), 
			@MaxRequestTime = MAX(P.PageRequestTime),
			@TotalRequests = COUNT(*) 
		FROM [dbo].tblPageRequestTime AS P
		WHERE P.[GUID] = @GUID;

		SELECT TOP 1
			S.SessionCount, 
			S.AvgStateLength, 
			S.MinStateLength, 
			S.MaxStateLength, 	
			S.AvgCompressionRate, 
			S.AvgSessionDuration, 
			S.CountExpired,
			@AverageRequestTime AS AverageRequestTime,
			@totalRequestTime AS TotalRequestTime,
			@MaxRequestTime AS MaxRequestTime,
			@TotalRequests AS TotalRequests
		FROM 
			[dbo].tblIspSession_Stats AS S	
		WHERE 
			S.statid = 1 AND S.APP_KEY = @App_Key
	END
	ELSE BEGIN
		SELECT TOP 1
			S.SessionCount, 
			S.AvgStateLength, 
			S.MinStateLength, 
			S.MaxStateLength, 	
			S.AvgCompressionRate, 
			S.AvgSessionDuration, 
			S.CountExpired,
			0 AS AverageRequestTime,
			0 AS TotalRequestTime,
			0 AS MaxRequestTime,
			0 AS TotalRequests
		FROM 
			[dbo].tblIspSession_Stats AS S	
		WHERE 
			S.statid = 1 AND S.APP_KEY = @App_Key

	END
END
GO

CREATE PROC  [dbo].[pSessionUpdateStats]
AS
BEGIN
SET NOCOUNT ON;

	DECLARE @SessionCount int,
		@AvgStateLength int,
		@MaxStateLength int,
		@MinStateLength int,		
		@AvgSessionDuration int,
		@CountExpired int, @App_Key binary(16);

	DECLARE getApps  CURSOR	LOCAL FORWARD_ONLY 
	FOR 	SELECT APP_KEY
		FROM [dbo].tblSessions_ISP GROUP BY APP_KEY;
	

	OPEN getApps;

	WHILE (1=1) BEGIN

		FETCH FROM getApps INTO @App_Key;
		IF @@FETCH_STATUS !=0 BREAK;

		IF NOT EXISTS (SELECT statid FROM [dbo].[tblIspSession_Stats]
					WHERE statid=1 AND APP_KEY = @App_Key) BEGIN

			INSERT INTO [dbo].[tblIspSession_Stats]
                      (statid, APP_KEY, SessionCount, AvgStateLength, MinStateLength, MaxStateLength, AvgCompressionRate, AvgSessionDuration, CountExpired)
			VALUES     (1,@App_Key, 0, 0, 0, 0, 0,  0, 0);
		END


		SELECT @SessionCount = COUNT(GUID) , 
		@AvgStateLength = ISNULL( AVG(DATALENGTH(I.State)), 0), 
		@MaxStateLength = ISNULL( MAX(DATALENGTH(I.State)), 0),  
		@MinStateLength = ISNULL( MIN(DATALENGTH(I.State)), 0), 
		@AvgSessionDuration = ISNULL( AVG(DATEDIFF(minute, I.Started, I.LastUpdated)), 0),
		@CountExpired = ISNULL(
						SUM(
								CASE 
								WHEN DATEDIFF(minute, I.LastUpdated,GetDate()) > I.Expires THEN 1
								ELSE 0 END), 0)
		FROM [dbo].TBLSESSIONS_ISP AS I		
		WHERE I.APP_KEY = @App_Key AND I.State IS NOT NULL;

--		PRINT @SessionCount;
--		PRINT @AvgStateLength;
--		PRINT @MaxStateLength;
--		PRINT @MinStateLength;
--		PRINT @AvgCompressionRate;
--		PRINT @AvgSessionDuration;
--		PRINT @CountExpired;

		UPDATE [dbo].[tblIspSession_Stats]
		Set SessionCount = @SessionCount, 
		AvgStateLength = @AvgStateLength, 
		MaxStateLength = @MaxStateLength, 
		MinStateLength = @MinStateLength,	
		AvgCompressionRate = 1,
		AvgSessionDuration = @AvgSessionDuration,
		CountExpired = @CountExpired
		WHERE statid=1 AND APP_KEY = @App_Key;

	END
	CLOSE getApps;
	DEALLOCATE getApps;

END


GO

CREATE PROC  [dbo].pSessionDelete
(
	@App_Key binary(16),
	@GUID binary(16)
)
AS	
	DELETE [dbo].tblSessions_ISP
	WHERE [GUID] = @GUID
	AND App_Key = @App_Key;
	DELETE [dbo].tblPageRequestTime
	WHERE [GUID] = @GUID

GO

-- Created may May 23, 2004
--Optionally schedule this stored proc if you do not want a Win32 service to do 

--this for you (see service\aspsession.exe)
CREATE PROC  [dbo].[proc_DeleteSessions] AS
BEGIN

	DECLARE @APP_KEY binary(16), @GUID binary(16)


	SET NOCOUNT ON;
	EXEC dbo.pSessionUpdateStats
	DECLARE ispsession_cursor CURSOR LOCAL FAST_FORWARD FOR
	  SELECT APP_KEY, GUID FROM [dbo].TBLSESSIONS_ISP WHERE 

		DateDiff(minute, LastUpdated, GetDate()) > Expires

	OPEN ispsession_cursor;

	WHILE (1=1) BEGIN
		FETCH NEXT FROM ispsession_cursor INTO @APP_KEY, @GUID 
		IF @@FETCH_STATUS != 0 BREAK;
		EXEC pSessionDelete @APP_KEY, @GUID;
	END

	CLOSE ispsession_cursor;
	DEALLOCATE ispsession_cursor;
END
GO
-- called by NWCSession.SessionPing
CREATE PROC  [dbo].SessionPing
	@App_Key binary(16),
	@GUID binary(16),
	@updateTo datetime	
AS
BEGIN	
	SET NOCOUNT ON;
	UPDATE [dbo].tblSessions_ISP SET LastUpdated=@updateTo
	 WHERE [GUID]=@GUID AND APP_KEY=@App_Key    
END
GO


-- ================================================
-- This will be called by the ASP Session Service scheduler ONLY if you have enabled this
-- ================================================
CREATE PROC  [dbo].[FixSecurityForTempDb]	
AS
BEGIN
	
	SET NOCOUNT ON;

	
	--EXEC  sp_executesql @statement= 
	--		N'use tempdb; IF NOT EXISTS (SELECT * FROM [tempdb].sys.sysusers where name =''aspsession'') BEGIN EXEC sp_adduser ''aspsession'' END ; exec sp_addrolemember db_datareader, aspsession; exec sp_addrolemember db_datawriter, aspsession';
	  
END
GO


GRANT EXECUTE ON dbo.pSessionInsert TO aspsession;
GRANT EXECUTE ON dbo.pSessionFetch TO aspsession;
GRANT EXECUTE ON dbo.pSessionSave TO aspsession;
GRANT EXECUTE ON dbo.FetchAllTimeOut TO aspsession;
GRANT EXECUTE ON dbo.proc_DeleteSessions TO aspsession;
GRANT EXECUTE ON dbo.pSessionDelete TO aspsession;	
GRANT EXECUTE ON dbo.pSessionGetStats to aspsession; 
GRANT EXECUTE ON dbo.pSessionUpdateStats to aspsession; 
GRANT EXECUTE ON dbo.SessionPing to aspsession;
GRANT EXECUTE ON dbo.CreateTempTables to aspsession;
GRANT EXECUTE ON [dbo].[FixSecurityForTempDb] to aspsession;

GO