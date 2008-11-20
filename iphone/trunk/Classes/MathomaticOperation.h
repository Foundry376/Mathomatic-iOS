//
//  MathomaticOperation.h
//  mathomatic
//
//  Created by Ben Gotow on 11/20/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface MathomaticOperation : NSObject {
    
    NSMutableArray * inputs;
    NSMutableArray * steps;
    NSMutableArray * stepNames;
    NSMutableArray * outputs;

    NSString * name;
}

@property (nonatomic, retain) NSMutableArray * inputs;
@property (nonatomic, retain) NSMutableArray * steps;
@property (nonatomic, retain) NSMutableArray * stepNames;
@property (nonatomic, retain) NSMutableArray * outputs;
@property (nonatomic, retain) NSString * name;

- (id)init;
- (void)addInput:(MathomaticExpression*)expression;
- (void)addStep:(NSString*)command withName:(NSString*)n;

@end
